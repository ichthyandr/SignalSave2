#include "packets.h"
#include <QString>

namespace MCU{

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

static bool is_little_endian(){
    volatile uint32_t i=0x01234567;
    return (*((uint8_t*)(&i))) == 0x67;
}

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

/*
static
uint32_t * swap32( float * data = nullptr ){
    if ( nullptr != data ){
        // данные в канале - little-endian, для big-endian вм переворачиваем
        if ( false == is_little_endian() ){
            float _data = *data;
            *data = ((((_data) & 0xff000000) >> 24) | (((_data) & 0x00ff0000) >>  8) | (((_data) & 0x0000ff00) <<  8) | (((_data) & 0x000000ff) << 24));
        }
    }
    return(data);
}
*/

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

uint16_t crc16_update_blk( uint16_t crc , uint8_t* a , uint32_t len ){
    uint8_t* ptr = a;
    while( len-- )
        crc = crc16_update_poly( crc , *ptr++ );
    return crc;
}

bool decode_SDU_AUTONOMOUS_ACK( const QByteArray & sdu, SDU_AUTONOMOUS_ACK & ack ){
    // контроль длины
    if ( sdu.size() != sizeof(SDU_AUTONOMOUS_ACK) ) return (false);
    // контроль тэга
    if ( AUTONOMOUS_ACK != sdu.at(0) ) return (false);
    ::memcpy( &ack, sdu.constData(), sdu.size());
    swap16( &ack.m_N );
    swap16( &ack.m_error );
    return (true);
}

bool decode_SDU_MYOGRAPH_ACK( const QByteArray & sdu, SDU_MYOGRAPH_ACK & ack ){
    // контроль длины
    if ( sdu.size() != sizeof(SDU_MYOGRAPH_ACK) ) return (false);
    // контроль тэга
    if ( MYOGRAPH_ACK != sdu.at(0) ) return (false);
    ::memcpy( &ack, sdu.constData(), sdu.size());
    swap16( &ack.m_N );
    swap16( &ack.m_error );
    return (true);
}

bool decode_SDU_MYOGRAPH_DATA( const QByteArray & sdu, SDU_MYOGRAPH_DATA & data ){
    // контроль длины
    if ( sdu.size() != sizeof(SDU_MYOGRAPH_DATA) ) return (false);
    // контроль тэга
    if ( MYOGRAPH_DATA != sdu.at(0) ) return (false);
    ::memcpy( &data, sdu.constData(), sdu.size());
    swap16( &data.m_N );
    swap16( &data.m_F );
    for ( int i = 0; i < 8 ; i++ ) swap32((uint32_t*)&data.m_data[i]);
    return (true);
}

bool decode_SDU_LEARNING_ACK( const QByteArray & sdu, SDU_LEARNING_ACK & ack ){
    // контроль длины
    if ( sdu.size() != sizeof(SDU_LEARNING_ACK) ) return (false);
    // контроль тэга
    if ( LEARNING_ACK != sdu.at(0) ) return (false);
    ::memcpy( &ack, sdu.constData(), sdu.size());
    swap16(&ack.m_N);
    swap16(&ack.m_amount_classes);
    swap16(&ack.m_amount_vectors);
    swap16(&ack.m_error);
    swap16(&ack.m_reserve);

    return (true);
}

bool decode_SDU_LEARNING_VECTOR_ACK(const QByteArray & sdu, SDU_LEARNING_VECTOR_ACK & ack ){
    // контроль длины
    if ( sdu.size() != sizeof(SDU_LEARNING_VECTOR_ACK) ) return (false);
    // контроль тэга
    if ( LEARNING_VECTOR_ACK != sdu.at(0) ) return (false);
    ::memcpy( &ack, sdu.constData(), sdu.size());
    swap16(&ack.m_N);
    swap16(&ack.m_error);
    return(true);
}

bool decode_SDU_LEARNING_PARAMS_ACK(const QByteArray & sdu, SDU_LEARNING_PARAMS_ACK & ack ){
    // контроль длины
    if ( sdu.size() != sizeof(SDU_LEARNING_PARAMS_ACK) ) return (false);
    // контроль тэга
    if ( LEARNING_PARAMS_ACK != sdu.at(0) ) return (false);
    ::memcpy( &ack, sdu.constData(), sdu.size());
    swap16(&ack.m_N);
    swap16(&ack.m_error);
    return(true);
}

bool decode_SDU_TESTING_ACK(const QByteArray & sdu, SDU_TESTING_ACK &ack ){
    // контроль длины
    if ( sdu.size() != sizeof(SDU_TESTING_ACK) ) return (false);
    // контроль тэга
    if ( TESTING_ACK != sdu.at(0) ) return (false);
    ::memcpy( &ack, sdu.constData(), sdu.size());
    swap16(&ack.m_N);
    swap16(&ack.m_error);
    return (true);
}

bool decode_SDU_TESTING_DATA(const QByteArray & sdu, SDU_TESTING_DATA &data ){
    // контроль длины
    if ( sdu.size() != sizeof(SDU_TESTING_DATA) ) return (false);
    // контроль тэга
    if ( TESTING_DATA != sdu.at(0) ) return (false);
    ::memcpy( &data, sdu.constData(), sdu.size());
    swap16(&data.m_N);
    swap16(&data.m_gest);
    swap16(&data.m_window);
    swap16(&data.m_offset);
    swap16(&data.m_average);
    swap16(&data.m_reserve);
    return (true);
}

void encode_SDU_AUTONOMOUS_CMD( const SDU_AUTONOMOUS_CMD & cmd, QByteArray & sdu ){
    SDU_AUTONOMOUS_CMD _cmd = cmd;
    _cmd.m_C = AUTONOMOUS;
    swap16(&_cmd.m_N);
    swap16(&_cmd.m_reserve);
    sdu = QByteArray((const char *)&_cmd, sizeof(_cmd));
}

void encode_SDU_MYOGRAPH_CMD( const SDU_MYOGRAPH_CMD & cmd, QByteArray & sdu ){
    SDU_MYOGRAPH_CMD _cmd = cmd;
    _cmd.m_C = MYOGRAPH;
    swap16(&_cmd.m_N);
    swap16(&_cmd.m_reserve);
    sdu = QByteArray((const char *)&_cmd, sizeof(_cmd));
}

void encode_SDU_LEARNING_CMD( const SDU_LEARNING_CMD & cmd, QByteArray & sdu ){
    SDU_LEARNING_CMD _cmd = cmd;
    _cmd.m_C = LEARNING;
    swap16(&_cmd.m_N);
    swap16(&_cmd.m_amount_classes);
    swap16(&_cmd.m_amount_vectors);
    swap16(&_cmd.m_reserve);
    sdu = QByteArray((const char *)&_cmd, sizeof(_cmd));
}

void encode_SDU_LEARNING_VECTOR_CMD( const SDU_LEARNING_VECTOR_CMD & cmd, QByteArray & sdu ){
    SDU_LEARNING_VECTOR_CMD _cmd = cmd;
    _cmd.m_C = LEARNING_VECTOR;
    swap16(&_cmd.m_N);
    swap16(&_cmd.m_class_number);
    swap16(&_cmd.m_vector_number);

    swap32((uint32_t*)&_cmd.m_channel0);
    swap32((uint32_t*)&_cmd.m_channel1);
    swap32((uint32_t*)&_cmd.m_channel2);
    swap32((uint32_t*)&_cmd.m_channel3);
    swap32((uint32_t*)&_cmd.m_channel4);
    swap32((uint32_t*)&_cmd.m_channel5);
    swap32((uint32_t*)&_cmd.m_channel6);
    swap32((uint32_t*)&_cmd.m_channel7);

    sdu = QByteArray((const char *)&_cmd, sizeof(_cmd));
}

void encode_SDU_LEARNING_PARAMS_CMD( const SDU_LEARNING_PARAMS_CMD & cmd, QByteArray & sdu ){
    SDU_LEARNING_PARAMS_CMD _cmd = cmd;
    _cmd.m_C = LEARNING_VECTOR;
    swap16(&_cmd.m_N);

    swap32((uint32_t*)&_cmd.m_K);
    swap32((uint32_t*)&_cmd.m_r);
    swap32((uint32_t*)&_cmd.m_B);

    swap16(&_cmd.m_Window);
    swap16(&_cmd.m_Shift);

    sdu = QByteArray((const char *)&_cmd, sizeof(_cmd));
}

void encode_SDU_TESTING_CMD( const SDU_TESTING_CMD & cmd, QByteArray & sdu ){
    SDU_TESTING_CMD _cmd = cmd;
    _cmd.m_C = TESTING;
    swap16(&_cmd.m_N);
    swap16(&_cmd.m_window);
    swap16(&_cmd.m_offset);
    swap16(&_cmd.m_average);
    swap16(&_cmd.m_reserve);
    sdu = QByteArray((const char *)&_cmd, sizeof(_cmd));
}

QByteArray encodePDU( const QByteArray & sdu ){

    QByteArray  out;
    uint16_t    length  = sdu.size();
    uint16_t    crc16   = 0xFFFF;

    out.append((const char *)swap16(&length),sizeof(length));
    out += sdu;

    for( auto byte: out ){
        crc16 = crc16_update_poly( crc16 , byte );
    }

    out.append((const char *)swap16(&crc16),sizeof(crc16));
    out.push_front(token_L);
    out.push_front(token_B);

    return (out);
}

/// SDU_AUTONOMOUS_CMD //////////////////////////////////////////////////////////////////
SDU_AUTONOMOUS_CMD::SDU_AUTONOMOUS_CMD(){}
SDU_AUTONOMOUS_CMD::SDU_AUTONOMOUS_CMD(const SDU_AUTONOMOUS_CMD &other){
    ::memcpy( this, &other, sizeof(other));
}
SDU_AUTONOMOUS_CMD::~SDU_AUTONOMOUS_CMD(){}

/// SDU_MYOGRAPH_CMD ////////////////////////////////////////////////////////////////////
SDU_MYOGRAPH_CMD::SDU_MYOGRAPH_CMD(){}
SDU_MYOGRAPH_CMD::SDU_MYOGRAPH_CMD(const SDU_MYOGRAPH_CMD &other){
    ::memcpy( this, &other, sizeof(other));
}
SDU_MYOGRAPH_CMD::~SDU_MYOGRAPH_CMD(){}

/// SDU_LEARNING_CMD ////////////////////////////////////////////////////////////////////
SDU_LEARNING_CMD::SDU_LEARNING_CMD(){}
SDU_LEARNING_CMD::SDU_LEARNING_CMD(const SDU_LEARNING_CMD &other){
    ::memcpy( this, &other, sizeof(other));
}
SDU_LEARNING_CMD::~SDU_LEARNING_CMD(){}

/// SDU_LEARNING_VECTOR_CMD /////////////////////////////////////////////////////////////
SDU_LEARNING_VECTOR_CMD::SDU_LEARNING_VECTOR_CMD(){}
SDU_LEARNING_VECTOR_CMD::SDU_LEARNING_VECTOR_CMD(const SDU_LEARNING_VECTOR_CMD &other){
    ::memcpy( this, &other, sizeof(other));
}
SDU_LEARNING_VECTOR_CMD::~SDU_LEARNING_VECTOR_CMD(){}

/// SDU_LEARNING_VECTOR_CMD /////////////////////////////////////////////////////////////
SDU_LEARNING_PARAMS_CMD::SDU_LEARNING_PARAMS_CMD(){}
SDU_LEARNING_PARAMS_CMD::SDU_LEARNING_PARAMS_CMD(const SDU_LEARNING_PARAMS_CMD &other){
    ::memcpy( this, &other, sizeof(other));
}
SDU_LEARNING_PARAMS_CMD::~SDU_LEARNING_PARAMS_CMD(){}

/// SDU_TESTING_CMD /////////////////////////////////////////////////////////////////////
SDU_TESTING_CMD::SDU_TESTING_CMD(){}
SDU_TESTING_CMD::SDU_TESTING_CMD(const SDU_TESTING_CMD &other){
    ::memcpy( this, &other, sizeof(other));
}
SDU_TESTING_CMD::~SDU_TESTING_CMD(){}

/// SDU_AUTONOMOUS_ACK //////////////////////////////////////////////////////////////////
SDU_AUTONOMOUS_ACK::SDU_AUTONOMOUS_ACK(){}
SDU_AUTONOMOUS_ACK::SDU_AUTONOMOUS_ACK(const SDU_AUTONOMOUS_ACK &other){
    ::memcpy( this, &other, sizeof(other));
}
SDU_AUTONOMOUS_ACK::~SDU_AUTONOMOUS_ACK(){}

/// SDU_MYOGRAPH_ACK ////////////////////////////////////////////////////////////////////
SDU_MYOGRAPH_ACK::SDU_MYOGRAPH_ACK(){}
SDU_MYOGRAPH_ACK::SDU_MYOGRAPH_ACK(const SDU_MYOGRAPH_ACK &other){
    ::memcpy( this, &other, sizeof(other));
}
SDU_MYOGRAPH_ACK::~SDU_MYOGRAPH_ACK(){}

/// SDU_MYOGRAPH_DATA ///////////////////////////////////////////////////////////////////
SDU_MYOGRAPH_DATA::SDU_MYOGRAPH_DATA(){}
SDU_MYOGRAPH_DATA::SDU_MYOGRAPH_DATA(const SDU_MYOGRAPH_DATA &other){
    ::memcpy( this, &other, sizeof(other));
}
SDU_MYOGRAPH_DATA::~SDU_MYOGRAPH_DATA(){}

/// SDU_LEARNING_ACK ////////////////////////////////////////////////////////////////////
SDU_LEARNING_ACK::SDU_LEARNING_ACK(){}
SDU_LEARNING_ACK::SDU_LEARNING_ACK(const SDU_LEARNING_ACK &other){
    ::memcpy( this, &other, sizeof(other));
}
SDU_LEARNING_ACK::~SDU_LEARNING_ACK(){}

/// SDU_LEARNING_VECTOR_ACK /////////////////////////////////////////////////////////////
SDU_LEARNING_VECTOR_ACK::SDU_LEARNING_VECTOR_ACK(){}
SDU_LEARNING_VECTOR_ACK::SDU_LEARNING_VECTOR_ACK(const SDU_LEARNING_VECTOR_ACK &other){
    ::memcpy( this, &other, sizeof(other));
}
SDU_LEARNING_VECTOR_ACK::~SDU_LEARNING_VECTOR_ACK(){}

/// SDU_LEARNING_PARAMS_ACK /////////////////////////////////////////////////////////////
SDU_LEARNING_PARAMS_ACK::SDU_LEARNING_PARAMS_ACK(){}
SDU_LEARNING_PARAMS_ACK::SDU_LEARNING_PARAMS_ACK(const SDU_LEARNING_PARAMS_ACK &other){
    ::memcpy( this, &other, sizeof(other));
}
SDU_LEARNING_PARAMS_ACK::~SDU_LEARNING_PARAMS_ACK(){}

/// SDU_TESTING_ACK /////////////////////////////////////////////////////////////////////
SDU_TESTING_ACK::SDU_TESTING_ACK(){}
SDU_TESTING_ACK::SDU_TESTING_ACK(const SDU_TESTING_ACK &other){
    ::memcpy( this, &other, sizeof(other));
}
SDU_TESTING_ACK::~SDU_TESTING_ACK(){}

/// SDU_TESTING_DATA ////////////////////////////////////////////////////////////////////
SDU_TESTING_DATA::SDU_TESTING_DATA(){}
SDU_TESTING_DATA::SDU_TESTING_DATA(const SDU_TESTING_DATA &other){
    ::memcpy( this, &other, sizeof(other));
}
SDU_TESTING_DATA::~SDU_TESTING_DATA(){}

///
QDebug& operator<<(QDebug& dbg, const SDU_AUTONOMOUS_CMD &cmd ){
    dbg.nospace() << QString("SDU_AUTONOMOUS_CMD: C=0x%1 N=%2 rsrve=%3").arg((int)cmd.m_C, 2, 16, QChar('0')).arg(cmd.m_N).arg(cmd.m_reserve);
    return dbg.maybeSpace();
}
QDebug& operator<<(QDebug& dbg, const SDU_MYOGRAPH_CMD &cmd ){
    dbg.nospace() << QString("SDU_MYOGRAPH_CMD: C=0x%1 N=%2 rsrve=%3").arg((int)cmd.m_C, 2, 16, QChar('0')).arg(cmd.m_N).arg(cmd.m_reserve);
    return dbg.maybeSpace();
}
QDebug& operator<<(QDebug& dbg, const SDU_LEARNING_CMD &cmd ){
    dbg.nospace() << QString("SDU_LEARNING_CMD: C=0x%1 N=%2 classes=%3 vectors=%4 rsrve=%5")
                     .arg((int)cmd.m_C, 2, 16, QChar('0'))
                     .arg(cmd.m_N)
                     .arg(cmd.m_amount_classes)
                     .arg(cmd.m_amount_vectors)
                     .arg(cmd.m_reserve);
    return dbg.maybeSpace();
}

QDebug& operator<<(QDebug& dbg, const SDU_LEARNING_VECTOR_CMD &cmd ){
    dbg.nospace() << QString("SDU_LEARNING_VECTOR_CMD: C=0x%1 N=%2 class=%3 vector=%4 ch0=%5 ch1=%6 ch2=%7 ch3=%8 ch4=%9 ch5=%10 ch6=%11 ch7=%12")
                     .arg((int)cmd.m_C, 2, 16, QChar('0'))
                     .arg(cmd.m_N)
                     .arg(cmd.m_class_number)
                     .arg(cmd.m_vector_number)
                     .arg(cmd.m_channel0)
                     .arg(cmd.m_channel1)
                     .arg(cmd.m_channel2)
                     .arg(cmd.m_channel3)
                     .arg(cmd.m_channel4)
                     .arg(cmd.m_channel5)
                     .arg(cmd.m_channel6)
                     .arg(cmd.m_channel7);
    return dbg.maybeSpace();
}

QDebug& operator<<(QDebug& dbg, const SDU_LEARNING_PARAMS_CMD &cmd ){
    dbg.nospace() << QString("SDU_LEARNING_PARAMS_CMD: C=0x%1 N=%2 K=%3 r=%4 Beta=%5 window=%6 shift=%7")
                     .arg((int)cmd.m_C, 2, 16, QChar('0'))
                     .arg(cmd.m_N)
                     .arg(cmd.m_K)
                     .arg(cmd.m_r)
                     .arg(cmd.m_B)
                     .arg(cmd.m_Window)
                     .arg(cmd.m_Shift);
    return dbg.maybeSpace();
}

QDebug& operator<<(QDebug& dbg, const SDU_TESTING_CMD &cmd ){
    dbg.nospace() << QString("SDU_LEARNING_CMD: C=0x%1 N=%2 wind=%3 offs=%4 avrg=%5 rsrve=%6")
                     .arg((int)cmd.m_C, 2, 16, QChar('0'))
                     .arg(cmd.m_N)
                     .arg(cmd.m_window)
                     .arg(cmd.m_offset)
                     .arg(cmd.m_average)
                     .arg(cmd.m_reserve);
    return dbg.maybeSpace();
}
QDebug& operator<<(QDebug& dbg, const SDU_AUTONOMOUS_ACK &ack ){
    dbg.nospace() << QString("SDU_AUTONOMOUS_ACK: D=0x%1 N=%2 err=%3")
                     .arg((int)ack.m_D, 2, 16, QChar('0'))
                     .arg(ack.m_N)
                     .arg(ack.m_error);
    return dbg.maybeSpace();
}
QDebug& operator<<(QDebug& dbg, const SDU_MYOGRAPH_ACK &ack ){
    dbg.nospace() << QString("SDU_MYOGRAPH_ACK: D=0x%1 N=%2 erre=%3")
                     .arg((int)ack.m_D, 2, 16, QChar('0'))
                     .arg(ack.m_N)
                     .arg(ack.m_error);
    return dbg.maybeSpace();
}
QDebug& operator<<(QDebug& dbg, const SDU_MYOGRAPH_DATA &data){
    dbg.nospace() << QString("SDU_MYOGRAPH_DATA: D=0x%1 N=%2 F=%3 %4 %5 %6 %7 %8 %9 %10 %11")
                     .arg((int)data.m_D, 2, 16, QChar('0'))
                     .arg(data.m_N)
                     .arg(data.m_F)
                     .arg(data.m_data[0])
                     .arg(data.m_data[1])
                     .arg(data.m_data[2])
                     .arg(data.m_data[3])
                     .arg(data.m_data[4])
                     .arg(data.m_data[5])
                     .arg(data.m_data[6])
                     .arg(data.m_data[7]);
    return dbg.maybeSpace();
}
QDebug& operator<<(QDebug& dbg, const SDU_LEARNING_ACK &ack ){
    dbg.nospace() << QString("SDU_LEARNING_ACK: D=0x%1 N=%2 classes=%3 vectors=%4 err=%5 rsrve=%6")
                     .arg((int)ack.m_D, 2, 16, QChar('0'))
                     .arg(ack.m_N)
                     .arg(ack.m_amount_classes)
                     .arg(ack.m_amount_vectors)
                     .arg(ack.m_error)
                     .arg(ack.m_reserve);
    return dbg.maybeSpace();
}
QDebug& operator<<(QDebug& dbg, const SDU_LEARNING_VECTOR_ACK &ack ){
    dbg.nospace() << QString("SDU_LEARNING_VECTOR_ACK: D=0x%1 N=%2 err=%3")
                     .arg((int)ack.m_D, 2, 16, QChar('0'))
                     .arg(ack.m_N)
                     .arg(ack.m_error);
    return dbg.maybeSpace();
}
QDebug& operator<<(QDebug& dbg, const SDU_LEARNING_PARAMS_ACK &ack ){
    dbg.nospace() << QString("SDU_LEARNING_PARAMS_ACK: D=0x%1 N=%2 err=%3")
                     .arg((int)ack.m_D, 2, 16, QChar('0'))
                     .arg(ack.m_N)
                     .arg(ack.m_error);
    return dbg.maybeSpace();
}
QDebug& operator<<(QDebug& dbg, const SDU_TESTING_ACK &ack ){
    dbg.nospace() << QString("SDU_TESTING_ACK: D=0x%1 N=%2 err=%3")
                     .arg((int)ack.m_D, 2, 16, QChar('0'))
                     .arg(ack.m_N)
                     .arg(ack.m_error);
    return dbg.maybeSpace();
}
QDebug& operator<<(QDebug& dbg, const SDU_TESTING_DATA &data ){
    dbg.nospace() << QString("SDU_TESTING_DATA: D=0x%1 N=%2 gest=%3 win=%4 offs=%5 avrg=%6 rsrve=%7")
                     .arg((int)data.m_D, 2, 16, QChar('0'))
                     .arg(data.m_N)
                     .arg(data.m_gest)
                     .arg(data.m_window)
                     .arg(data.m_offset)
                     .arg(data.m_average)
                     .arg(data.m_reserve);
    return dbg.maybeSpace();
}
///
QTextStream& operator<<(QTextStream& stream, const SDU_TESTING_DATA &data ){
    stream << QString("SDU_TESTING_DATA: D=0x%1 N=%2 gest=%3 win=%4 offs=%5 avrg=%6 rsrve=%7")
              .arg((int)data.m_D, 2, 16, QChar('0'))
              .arg(data.m_N)
              .arg(data.m_gest)
              .arg(data.m_window)
              .arg(data.m_offset)
              .arg(data.m_average)
              .arg(data.m_reserve);
    return stream;
}

QTextStream& operator<<(QTextStream& stream, const SDU_MYOGRAPH_DATA &data ){
    stream << QString("SDU_MYOGRAPH_DATA: D=0x%1 N=%2 F=%3 %4 %5 %6 %7 %8 %9 %10 %11")
              .arg((int)data.m_D, 2, 16, QChar('0'))
              .arg(data.m_N)
              .arg(data.m_F)
              .arg(data.m_data[0])
              .arg(data.m_data[1])
              .arg(data.m_data[2])
              .arg(data.m_data[3])
              .arg(data.m_data[4])
              .arg(data.m_data[5])
              .arg(data.m_data[6])
              .arg(data.m_data[7]);
    return stream;
}

QTextStream& operator<<(QTextStream& stream, const SDU_LEARNING_ACK &ack ){
    stream << QString("SDU_LEARNING_ACK: D=0x%1 N=%2 classes=%3 vectors=%4 err=%5 rsrve=%6")
              .arg((int)ack.m_D, 2, 16, QChar('0'))
              .arg(ack.m_N)
              .arg(ack.m_amount_classes)
              .arg(ack.m_amount_vectors)
              .arg(ack.m_error)
              .arg(ack.m_reserve);
    return stream;
}

QTextStream& operator<<(QTextStream& stream, const SDU_LEARNING_VECTOR_ACK &ack ){
    stream << QString("SDU_LEARNING_VECTOR_ACK: D=0x%1 N=%2 err=%3")
              .arg((int)ack.m_D, 2, 16, QChar('0'))
              .arg(ack.m_N)
              .arg(ack.m_error);
    return stream;
}

QTextStream& operator<<(QTextStream& stream, const SDU_LEARNING_PARAMS_ACK &ack ){
    stream << QString("SDU_LEARNING_PARAMS_ACK: D=0x%1 N=%2 err=%3")
              .arg((int)ack.m_D, 2, 16, QChar('0'))
              .arg(ack.m_N)
              .arg(ack.m_error);
    return stream;
}

///
}
