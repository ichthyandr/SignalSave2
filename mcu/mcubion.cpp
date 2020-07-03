#include "mcubion.h"

Q_LOGGING_CATEGORY(BION,  "BION")

namespace MCUBION {

static
QString hexdump( const QByteArray & msg ){
    QString out = QString("size=%1").arg(msg.size());
    for (uchar c : msg) {
    //qDebug() << QString("0x%1").arg((int)c, 0, 16);
        if ( out.size() ){
            out += QString(" 0x%1").arg(c, 2, 16, QLatin1Char('0'));
        }
        else {
            out += QString("0x%1").arg(c, 2, 16, QLatin1Char('0'));
        }
    }
    return (out);
}

static
bool is_little_endian(){
    volatile uint32_t i=0x01234567;
    return (*((uint8_t*)(&i))) == 0x67;
}

static
uint16_t * swap16( uint16_t * data = nullptr ){
    if ( nullptr != data ){
        // данные в канале - little-endian, для big-endian вм переворачиваем
        if ( false == is_little_endian() ){
            register uint16_t _data = *data;
            *data = ((((_data) & 0xff00) >> 8) | (((_data) & 0x00ff) << 8));
        }
    }
    return(data);
}

static
uint32_t * swap32( uint32_t * data = nullptr ){
    if ( nullptr != data ){
        // данные в канале - little-endian, для big-endian вм переворачиваем
        if ( false == is_little_endian() ){
            register uint32_t _data = *data;
            *data = ((((_data) & 0xff000000) >> 24) | (((_data) & 0x00ff0000) >>  8) | (((_data) & 0x0000ff00) <<  8) | (((_data) & 0x000000ff) << 24));
        }
    }
    return(data);
}

// декодировать uint16_t из байт-последовательности
static
uint16_t decodeUINT16( const QByteArray & data, bool * ok = nullptr){
    uint16_t res    = 0;
    bool _ok        = false;

    if ( data.length() >= 2 ){
        ::memcpy( &res, data.constData(), 2);
        swap16( &res );
        _ok = true;
    }
    if ( nullptr != ok ) *ok = _ok;

    return (res);
}

static
uint16_t crc16_update_poly( uint16_t crc , uint8_t a ){
    crc ^= a;
    uint_fast8_t i;
    for ( i = 0 ; i < 8 ; ++i )
    {
        if ( crc & 1 )
            crc = ( crc >> 1 ) ^ 0xA001;
        else
            crc = ( crc >> 1 );
    }
    return crc;
}

static
uint16_t crc16_update_blk( uint16_t crc , uint8_t* a , uint32_t len ){
    uint8_t* ptr = a;
    while( len-- )
        crc = crc16_update_poly( crc , *ptr++ );
    return crc;
}

//------------------------------------------------------------------------------
// serial worker
//------------------------------------------------------------------------------

serialWorker::serialWorker(const QString &portName, const qint32 baudRate, QObject *parent)
    : QObject(parent),
      m_bEnabled(false),
      m_portName(portName),
      m_baudRate(baudRate),
      m_serial( this ){
    qCDebug(BION) << Q_FUNC_INFO<< "portName=" << m_portName << " baudRate=" << baudRate;
}

serialWorker::~serialWorker(){
    qCDebug(BION) << Q_FUNC_INFO;
    _off();
}

void serialWorker::_on(){
    qCDebug(BION) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();

    if ( !m_bEnabled ){
        qCDebug(BION) << "try to start serial worker ... port=" << m_portName << " baud=" << m_baudRate;

        // Установка параметров
        m_serial.setPortName( m_portName );
        m_serial.setBaudRate( m_baudRate ); /*QSerialPort::Baud9600;*/
        m_serial.setDataBits( QSerialPort::Data8 );
        m_serial.setParity( QSerialPort::NoParity );
        m_serial.setStopBits( QSerialPort::OneStop );
        m_serial.setFlowControl( QSerialPort::NoFlowControl );

        // включение
        if (!m_serial.open(QIODevice::ReadWrite)) {
            qCCritical(BION) << Q_FUNC_INFO << " can't open port " << m_portName;
            emit s_status(QString::fromUtf8("ERROR: cant open port"));
            _off( );
            return;
        }

        qCDebug(BION) << "signals=" << m_serial.pinoutSignals();
        m_serial.clear();
        m_serial.setDataTerminalReady( true );
        m_serial.setRequestToSend( true );
        qCDebug(BION) << "signals=" << m_serial.pinoutSignals();

        // запускаем работу
        m_bEnabled = true;
        emit s_status(QString::fromUtf8("SUCCESS: port is opened"));

        connect( &m_serial, &QSerialPort::readyRead, this, &serialWorker::_get);
    }
    else {
        qCDebug(BION) << "serial worker already started!";
    }
}

void serialWorker::_off(){
    qCDebug(BION) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();
    if ( m_bEnabled ){
        qCDebug(BION) << Q_FUNC_INFO << "try to stop serial worker ...";
        if ( m_serial.isOpen() ) m_serial.close();
        qCDebug(BION) << Q_FUNC_INFO << "stopped ...";
        m_bEnabled = false;
    }
    else {
        qCDebug(BION) << Q_FUNC_INFO << "serial worker already stopped";
    }
}

QByteArray serialWorker::take_SDU(){

    QByteArray sdu;

    //////////////////////////////////////////////////////////////////
    /// 1. ищем в буфере PDU
    typedef enum _pdu_stm {
        pdu_token_B,
        pdu_token_L,
        pdu_length,
        pdu_payload,
        pdu_crc,
        pdu_done
    } pdu_stm_t;                        // состояния парсера

    pdu_stm_t state     = pdu_token_B;
    int start_char      = 0;            // первый символ PDU в буффере
    int len_length      = 2;            // длина поля Length
    int len_crc16       = 2;            // длина поля crc16
    int len_payload     = 0;            // длина поля payload

    QByteArray          _length;        // длина, принятая в PDU
    QByteArray          _payload;       // payload
    QByteArray          _crc16;         // crc16, принятый в PDU
    QByteArray          _frame;         // PDU

    for( int i = 0; i < m_buffer.size(); i++ ){
        switch ( state ){
        case pdu_token_B:
            if ( MCU::token_B == m_buffer.at(i) ){
                start_char = i;
                state = pdu_token_L;
                _frame+=m_buffer.at(i);
            }
            break;
        case pdu_token_L:
            if ( MCU::token_L == m_buffer.at(i) ){
                state = pdu_length;
                _frame+=m_buffer.at(i);
            }
            else {
                state = pdu_token_B;
                start_char = 0;
                _frame.clear();
            }
            break;
        case pdu_length:
            if ( len_length > 0){
                _length += m_buffer.at(i);
                len_length--;
                _frame+=m_buffer.at(i);
                if ( 0 == len_length ){
                    len_payload = decodeUINT16( _length );
                    if ( len_payload > 0 )
                        state = pdu_payload;
                    else
                        state = pdu_crc;
                }
            }
            break;
        case pdu_payload:
            if ( len_payload > 0){
                _payload += m_buffer.at(i);
                len_payload--;
                _frame+=m_buffer.at(i);
                if ( 0 == len_payload ){
                    state = pdu_crc;
                }
            }
            break;
        case pdu_crc:
            if ( len_crc16 > 0 ){
                _crc16 += m_buffer.at(i);
                len_crc16--;
                _frame+=m_buffer.at(i);
                if ( 0 == len_crc16 ){
                    state = pdu_done;
                }
            }
            break;
        case pdu_done:
            break;
        }
        if ( pdu_done == state ) break; // PDU найден, выход из цикла
    }

    //////////////////////////////////////////////////////////////////
    /// 2. проверяем и чистим буфер

    if ( pdu_done == state ){

        m_pdu_counter++;

        // удалить мусор перед PDU
        m_buffer.remove( 0, start_char );
        // удалить PDU из буфера
        m_buffer.remove( 0, _frame.size() );
        // сверка CRC16
        uint16_t crc16   = 0xFFFF;
        QByteArray _pdu;
        _pdu += _length;
        _pdu += _payload;

        for ( auto byte: _pdu ){
            crc16 = crc16_update_poly( crc16 , byte );
        }

        if ( crc16 == decodeUINT16(_crc16)){
            sdu = _payload;
        }
        else {
            m_crc_err_counter++;
            qCCritical(BION) << Q_FUNC_INFO << "bad crc16: " << hexdump(_frame);
        }
    }
    return (sdu);
}

void serialWorker::_get(){
    // читаем из порта
    qCDebug(BION) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();

    QByteArray resp = m_serial.readAll();
    //while (m_serial.waitForReadyRead(10)) resp += m_serial.readAll();

    if ( resp.size() ){
        m_buffer += resp;
        qCDebug(BION) << Q_FUNC_INFO << "recv: " << hexdump( resp ) << "m_buffer: " << hexdump(m_buffer);

        QByteArray sdu;
        do {
            sdu = take_SDU();
            if ( sdu.size() > 0 ) emit s_recv( sdu );
        } while ( sdu.size() > 0 );
        // контроль размера буфера
        if ( m_buffer.size() > m_buffMAX ){
            int len = m_buffer.size() - m_buffMAX;
            m_buffer.remove( 0 , len );
            qCCritical(BION) << Q_FUNC_INFO << "buffer is truncated by length=" << len;
        }
    }
}

//------------------------------------------------------------------------------
// Controller
//------------------------------------------------------------------------------
Controller::Controller( QObject *parent ):QObject(parent), m_thread(), m_enable(false){
    qCDebug(BION) << Q_FUNC_INFO;
}

Controller::~Controller(){
    qCDebug(BION) << Q_FUNC_INFO;
    off();
}

void Controller::on(const QString &portName, const qint32 baudRate, const int gain){
    qCDebug(BION) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();
    if ( !m_enable ){
        m_enable    = true;
        m_gain      = gain;

        qCDebug(BION) << Q_FUNC_INFO << "m_gain=" << gain;

        serialWorker * pWorker = new serialWorker( portName, baudRate );
        pWorker->moveToThread(&m_thread);

        connect(&m_thread,  &QThread::finished,     pWorker,    &QObject::deleteLater );
        connect(&m_thread,  &QThread::started,      pWorker,    &serialWorker::_on );
        connect(pWorker,    &serialWorker::s_recv,  this,       &Controller::_get);
        connect(pWorker,    &serialWorker::s_status,this,       &Controller::_status);

        m_thread.start();
        do {
            QThread::msleep(1);
            QThread::yieldCurrentThread();

        } while ( !m_thread.isRunning() );
    }
}

void Controller::off(){
    qCDebug(BION) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();
    if ( m_enable ){
        m_thread.quit();
        m_thread.wait();
        m_enable = false;
        emit s_state ("SUCCESS: stopped");
    }
}

void Controller::_get( const QByteArray & msg ){
    //qDebug() << Q_FUNC_INFO << "thread ID=" << QThread::currentThreadId();
    ///////////////////////////////////
    /// декодировать SDU здесь

    if ( m_enable ) {
        MCU::SDU_MYOGRAPH_DATA sdu;
        bool ret =  MCU::decode_SDU_MYOGRAPH_DATA( msg, sdu );
        if ( ret ) {
            for( int i=0; i<ELECTRODES_AMOUNT; i++ ) sdu.m_data[i] *= m_gain;
            qCDebug(BION) << Q_FUNC_INFO << sdu;
            emit s_myograph_data( sdu );
        }
        else {
            qCCritical(BION) << Q_FUNC_INFO << "can't decode SDU, decode_SDU_MYOGRAPH_DATA failed!";
        }
    }
    else
        qCCritical(BION) << Q_FUNC_INFO << "Controller is disabled!";
}

void Controller::_status( const QString & msg ){
    qCDebug(BION) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();
    emit s_state( msg );
}

bool Controller::isEnabled() const {
    qCDebug(BION) << Q_FUNC_INFO << " m_enable=" << m_enable;
    return( m_enable );
}

}
