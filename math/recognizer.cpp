/*utf8*/
#include "recognizer.h"
#include <QFile>
#include <QtMath>
Q_LOGGING_CATEGORY(RCG, "RCG")

//#define _USE_MATH_DEFINES // for C++
//#include <cmath>

namespace MATH {

// ф-ция вычисления среднего по массиву
float average( QVector<float> & data ){
    int N       = data.size();
    float res   = 0.0f;
    float summ  = 0.0f;
    for( auto & W: data){
        summ += W;
    }
    res = summ/N;
    return(res);
}


// ф-ция вычисления дисперсии по массиву и среднему
float dispersion( QVector<float> & data, float average ){
    int N       = data.size();
    float res   = 0.0f;
    float summ  = 0.0f;
    for( float W: data){
        summ += (W - average)*(W - average);
    }
    res = summ/(N-1);
    return(res);
}

// ф-ция вычисления сигмоиды
float sigma( float D, float K, float r, float beta ){
    //qCDebug(RCG) << Q_FUNC_INFO << "D=" << D << "K=" << K << "r=" << r << "beta=" << beta;
    float d     = (D*D/K)-r;    // дисперсия возводится в квадрат !

    //qCDebug(RCG) << Q_FUNC_INFO << "d=" << d;
    //qCDebug(RCG) << Q_FUNC_INFO << "exp=" << exp(-beta*d);
    //qCDebug(RCG) << Q_FUNC_INFO << "pow=" << pow(M_E,-beta*d);

    float res   = 1.0f / (1 + exp(-beta*d));
    //float res   = (1.0f/(1.0f + pow(M_E,-beta*d)));
    return (res);
}

/// ф-ция умножения вектора на хэш матрицу
static
void multiplication( float (&_matrix)[4][8], float (&_vector)[8], float (&_result)[4]){

    _result[0] = _matrix[0][0]*_vector[0]+_matrix[0][1]*_vector[1]+_matrix[0][2]*_vector[2]+_matrix[0][3]*_vector[3]+_matrix[0][4]*_vector[4]+_matrix[0][5]*_vector[5]+_matrix[0][6]*_vector[6]+_matrix[0][7]*_vector[7];
    _result[1] = _matrix[1][0]*_vector[0]+_matrix[1][1]*_vector[1]+_matrix[1][2]*_vector[2]+_matrix[1][3]*_vector[3]+_matrix[1][4]*_vector[4]+_matrix[1][5]*_vector[5]+_matrix[1][6]*_vector[6]+_matrix[1][7]*_vector[7];
    _result[2] = _matrix[2][0]*_vector[0]+_matrix[2][1]*_vector[1]+_matrix[2][2]*_vector[2]+_matrix[2][3]*_vector[3]+_matrix[2][4]*_vector[4]+_matrix[2][5]*_vector[5]+_matrix[2][6]*_vector[6]+_matrix[2][7]*_vector[7];
    _result[3] = _matrix[3][0]*_vector[0]+_matrix[3][1]*_vector[1]+_matrix[3][2]*_vector[2]+_matrix[3][3]*_vector[3]+_matrix[3][4]*_vector[4]+_matrix[3][5]*_vector[5]+_matrix[3][6]*_vector[6]+_matrix[3][7]*_vector[7];

}

//////////////////////////////////////////////////////////////////////
/// подкласс скользящего окна
Recognizer::slidingWindow::slidingWindow(){
}

Recognizer::slidingWindow::~slidingWindow(){
}

void Recognizer::slidingWindow::setUp(const int window, const int shift ){
    qCDebug(RCG) << Q_FUNC_INFO << "window=" << window << "shift=" << shift;
    // проверка входных параматров
    bool error = false;

    if (!(window > shift)){
        error = true;
        qCCritical(RCG) << Q_FUNC_INFO << QString::fromUtf8("размер окна должен быть больше смещения");
    }
    else if ( !(shift > 0) ) {
        error = true;
        qCCritical(RCG) << Q_FUNC_INFO << QString::fromUtf8("смещение должно быть больше нуля");
    }
    if ( !error ){
        m_window    = window;
        m_shift     = shift;
        // чистим буфер
        m_buffer.clear();
    }
}

bool Recognizer::slidingWindow::packet( MCU::SDU_MYOGRAPH_DATA packet, QVector<MCU::SDU_MYOGRAPH_DATA> & frame ){
    m_buffer << packet;
    // если меньше размера окна, выход
    if ( m_buffer.size() < m_window ) return(false);
    // иначе буфер полный, передаем фрейм
    frame = m_buffer;
    // удаляем первые элементы количеством m_shift
    m_buffer.remove(0, m_shift );
    return( true );
}

//////////////////////////////////////////////////////////////////////
/// класс жеста
Recognizer::Class::Class( QVector<float> & data, int gest ):
    m_data(data), m_gest(gest){
    qCDebug(RCG) << Q_FUNC_INFO << "Vector" << m_data;
    qCDebug(RCG) << Q_FUNC_INFO << "gest" << m_gest;
}

//////////////////////////////////////////////////////////////////////
/// классификатор
void Recognizer::Classificator::Add( QVector<float> & data, int gest){
    Class cl( data, gest);
    m_classes << cl;
}

int Recognizer::Classificator::Recognize(QVector<float> & vect){
    int res = -1;
    QMap<float, int> map;
    for( Class cl: m_classes ){
        if ( cl.m_data.size() == vect.size() ){
            qCDebug(RCG) << Q_FUNC_INFO << cl.m_data;
            qCDebug(RCG) << Q_FUNC_INFO << vect;
            float distance = 0.0;
            for( int i=0; i<vect.size(); i++ ){
                float d = cl.m_data.at(i) - vect.at(i);
                distance += d*d;
            }
            distance = sqrt(distance);
            qCDebug(RCG) << Q_FUNC_INFO << "distance=" << distance << "gest=" << cl.m_gest;
            map.insert( distance, cl.m_gest );
        }
        else
            qCCritical(RCG) << Q_FUNC_INFO << "vectors have diff sizes!" << cl.m_data.size() << vect.size();
    }
    if ( !map.isEmpty()){
        qCDebug(RCG) << Q_FUNC_INFO << map;
        res = map.first();
    }
    return (res);
}

void Recognizer::Classificator::clear(){
    m_classes.clear();
}

//////////////////////////////////////////////////////////////////////
/// подкласс гистограммы
Recognizer::Histogram::Histogram( const int max ): m_max(max) {
    qCDebug(RCG) << Q_FUNC_INFO << "m_max=" << m_max;
    for(int i = 0; i<GESTURES_AMOUNT; i++) m_frequency[i] = 0;
}

int Recognizer::Histogram::weigh(int gest){
    // номера жестов от 1 до GESTURES_AMOUNT включительно !!!!
    if ((gest>=1) && (gest<=GESTURES_AMOUNT)){
        m_queue.enqueue( gest );
        // массив частот индексируется от 0 !!!!
        m_frequency[ gest-1 ]++;
        if (m_queue.size() > m_max ){
            int _gest = m_queue.dequeue();
            // массив частот индексируется от 0 !!!!
            m_frequency[ _gest-1 ]--;
        }
        // ищем индекс с максимальной частотой
        int maxIndex = 0;
        // искать начинаем от 1 !!!
        for(int i = 1; i<GESTURES_AMOUNT; i++)
            if( m_frequency[i] > m_frequency[maxIndex]) maxIndex = i;
        // жесты нумеруются с 1
        int gest = maxIndex + 1;
        return( gest );
    }
    else
        qCCritical(RCG) << Q_FUNC_INFO << "Bad gest number=" << gest;
    /// в случае ошибки возвращаем (-1)
    return(-1);
}

void Recognizer::Histogram::setUp(const int queue ){
    if ( queue > 0 ){
        qCDebug(RCG) << Q_FUNC_INFO << "histogram window=" << queue;
        m_max = queue;
        m_queue.clear();
        for(int i = 0; i<GESTURES_AMOUNT; i++) m_frequency[i] = 0;
    }
    else
        qCCritical(RCG) << Q_FUNC_INFO << QString::fromUtf8("Недопустимый размер окна гистограммы!") << queue;
}


//////////////////////////////////////////////////////////////////////
/// класс рекогнайзера
Recognizer::Recognizer(){
    qCDebug(RCG) << Q_FUNC_INFO;
}

Recognizer::Recognizer( const int window, const int shift, const float K, const float r, const float Beta, const int queue ){
    qCDebug(RCG) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();
    qCDebug(RCG) << Q_FUNC_INFO << "window=" << window << "shift=" << shift << "K=" << K << "r=" << r << "Beta=" << Beta << "queue=" << queue;
    setUp( window, shift, K, r, Beta, queue );
}

Recognizer::~Recognizer(){
    qCDebug(RCG) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();
    _stop();
}

void Recognizer::setUp( const int window, const int shift, const float K, const float r, const float Beta, const int queue ){
    m_slwin.setUp( window, shift );
    m_histogram.setUp( queue );

    if ( K != 0.0f )  m_K = K; else qCCritical(RCG) << Q_FUNC_INFO << "K has zero!";
    m_r = r;
    m_beta = Beta;
}

bool Recognizer::loadMatrix( QString path ){

    // загружаем матрицу из файла
    float       _matrix[4][8];
    QStringList lines;
    QFile       inputFile(path);

    if ( inputFile.open( QIODevice::ReadOnly | QIODevice::Text ) ){
        QTextStream in(&inputFile);
        while (!in.atEnd()){
           QString line = in.readLine();
           lines << line;
        }
        inputFile.close();
    }
    else {
        m_error = QString::fromUtf8("Не могу прочесть файл %1").arg( path );
        qCCritical(RCG) << Q_FUNC_INFO << m_error;
        m_bError = true;
    }

    if ( !m_bError ){
        if ( !(lines.size() < 4)){
            int i = 0;
            for (QString line : lines){
                if (i > 3) break;
                QRegExp sep("\\s+");
                QStringList row = line.split(sep);

                if ( row.size() < 8 ){
                    m_error = QString::fromUtf8("Строка %1 файла %2 содержит недопустимое кол-во элементов %3").arg(i+1).arg(path).arg(row.size());
                    qCCritical(RCG) << Q_FUNC_INFO << m_error;
                    m_bError = true;
                    break;
                }

                int j = 0;
                for (QString elem : row ){
                    if (j > 7) break;
                    QRegExp rx("^\\-{0,1}\\d{1}\\.\\d+$");
                    if ( elem.contains(rx)){
                        //elem.replace(QString(","), QString("."));

                        bool ok;
                        float el = elem.toFloat(&ok);

                        if ( ok ){
                            _matrix[i][j] = el;
                        }
                        else {
                            m_error = QString::fromUtf8("Не могу проебразовать эелемент %1 в double строка %2 столбец %3 файла %4").arg(elem).arg(i+1).arg(j+1).arg(path);
                            qCCritical(RCG) << Q_FUNC_INFO << m_error;
                            m_bError = true;
                            break;
                        }
                    }
                    else{
                        m_error = QString::fromUtf8("Неверный формат элемента %1 строка %2 столбец %3 файла %4").arg(elem).arg(i+1).arg(j+1).arg(path);
                        qCCritical(RCG) << Q_FUNC_INFO << m_error;
                        m_bError = true;
                        break;
                    }
                    j++;
                }
                if ( m_bError ) break;
                i++;
            }
            if ( !m_bError ){
                for (int i=0; i<4; i++ ){
                    for (int j=0; j<8; j++ ){
                        m_matrix[i][j] = _matrix[i][j];
                    }
                }
            }
        }
        else {
            m_error = QString::fromUtf8("Файл %1 содержит мало строк: %2").arg( path ).arg(lines.size());
            qCCritical(RCG) << Q_FUNC_INFO << m_error;
            m_bError = true;
        }
    }

    if (!m_bError){
        // вывод матрицы в дебаг лог
        qCDebug(RCG)<<Q_FUNC_INFO<<"LOADED MATRIX";
        qCDebug(RCG)<<Q_FUNC_INFO<<m_matrix[0][0]<<m_matrix[0][1]<<m_matrix[0][2]<<m_matrix[0][3]<<m_matrix[0][4]<<m_matrix[0][5]<<m_matrix[0][6]<<m_matrix[0][7];
        qCDebug(RCG)<<Q_FUNC_INFO<<m_matrix[1][0]<<m_matrix[1][1]<<m_matrix[1][2]<<m_matrix[1][3]<<m_matrix[1][4]<<m_matrix[1][5]<<m_matrix[1][6]<<m_matrix[1][7];
        qCDebug(RCG)<<Q_FUNC_INFO<<m_matrix[2][0]<<m_matrix[2][1]<<m_matrix[2][2]<<m_matrix[2][3]<<m_matrix[2][4]<<m_matrix[2][5]<<m_matrix[2][6]<<m_matrix[2][7];
        qCDebug(RCG)<<Q_FUNC_INFO<<m_matrix[3][0]<<m_matrix[3][1]<<m_matrix[3][2]<<m_matrix[3][3]<<m_matrix[3][4]<<m_matrix[3][5]<<m_matrix[3][6]<<m_matrix[3][7];
        // установить признак HASH рекогнайзера
        m_type = HASH;
    }

    return (!m_bError);
}

void Recognizer::_packet( MCU::SDU_MYOGRAPH_DATA packet ){
    if ( m_bEnabled ){
        qCDebug(RCG) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();
        qCDebug(RCG) << Q_FUNC_INFO << "<<<<<<<<<<<<<<<<<<<<";

        QVector<MCU::SDU_MYOGRAPH_DATA> frame;
        if ( m_slwin.packet( packet, frame ) ){
            GestData gestData;
            for ( MCU::SDU_MYOGRAPH_DATA packet: frame ){
                for ( int i = 0; i < ELECTRODES_AMOUNT; i++ ){
                    gestData.m_channel[i] <<  packet.m_data[i];
                }
            }
            // рассчет оцениваемой точки, вектор содержит рассчитанные сигмоиды
            QVector<float> sigmoids;
            for ( int i = 0; i < ELECTRODES_AMOUNT; i++ ){
                float AVG = average(gestData.m_channel[i]);
                float DISP= dispersion(gestData.m_channel[i], AVG);
                float SIGM= sigma( DISP, m_K, m_r, m_beta );
                sigmoids << SIGM;
            }

            int gest;
            switch (m_type) {
            case NEIGHBOR:
                gest = m_cls.Recognize( sigmoids );
                break;
            case HASH:
                if ( 8 == sigmoids.size() ){

                    float result[4]    = {0, 0, 0, 0};
                    float vect[8]      = {sigmoids.at(0), sigmoids.at(1), sigmoids.at(2), sigmoids.at(3), sigmoids.at(4), sigmoids.at(5), sigmoids.at(6), sigmoids.at(7)};

                    multiplication( m_matrix, vect, result );
                    QVector<float> data;
                    data << result[0] << result[1] << result[2] <<  result[3];
                    gest = m_cls.Recognize( data );
                }
                else
                    qCCritical(RCG) << Q_FUNC_INFO << "bad sigmoids size!";
                break;
            }

            int wGest = m_histogram.weigh( gest );
            if ( wGest > 0 ) {
                emit s_gest( wGest );
            }
            else {
                emit s_gest( gest );
                qCCritical(RCG) << Q_FUNC_INFO << "histogram error!";
            }
        }

        qCDebug(RCG) << Q_FUNC_INFO << ">>>>>>>>>>>>>>>>>>>>";
    }
}

void Recognizer::load(QVector<GestData> gestsDB){
    qCDebug(RCG) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();
    // очищаем классификатор
    m_cls.clear();
    // делаем рассчет и грузим в классификатор
    for( GestData & gestData: gestsDB ){

        qCDebug(RCG) << Q_FUNC_INFO << "gest=" << gestData.m_gest;
        qCDebug(RCG) << Q_FUNC_INFO << "number=" << gestData.m_number;
        qCDebug(RCG) << Q_FUNC_INFO << "size=" << gestData.m_channel[0].size();
        qCDebug(RCG) << Q_FUNC_INFO << "counter=" << gestData.m_counter;

        QVector<float> averages;
        QVector<float> dispersions;
        QVector<float> sigmoids;

        for ( int i=0; i<ELECTRODES_AMOUNT; i++ ){

            qCDebug(RCG) << Q_FUNC_INFO << "channel#" << i << gestData.m_channel[i];

            float AVG = average( gestData.m_channel[i] );
            float DISP = dispersion( gestData.m_channel[i], AVG );
            float SIGMA = sigma(DISP, m_K , m_r, m_beta );

            averages    << AVG;
            dispersions << DISP;
            sigmoids    << SIGMA;
        }
        qCDebug(RCG) << Q_FUNC_INFO << "AVERAGE" << averages;
        qCDebug(RCG) << Q_FUNC_INFO << "DISPERS" << dispersions;
        qCDebug(RCG) << Q_FUNC_INFO << "SIGMOID" << sigmoids;

        m_cls.Add( sigmoids, gestData.m_gest );

        qCDebug(RCG) << Q_FUNC_INFO << "-------------------------------------------------------";
    }
}

bool Recognizer::load( QString path, QVector<GestData> gestsDB ){
    qCDebug(RCG) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();
    bool res = loadMatrix(path);

    if (res){ // матрица загружена
        // очищаем классификатор
        m_cls.clear();
        // делаем рассчет и грузим в классификатор
        for( GestData & gestData: gestsDB ){

            qCDebug(RCG) << Q_FUNC_INFO << "gest=" << gestData.m_gest;
            qCDebug(RCG) << Q_FUNC_INFO << "number=" << gestData.m_number;
            qCDebug(RCG) << Q_FUNC_INFO << "size=" << gestData.m_channel[0].size();
            qCDebug(RCG) << Q_FUNC_INFO << "counter=" << gestData.m_counter;

            QVector<float> averages;
            QVector<float> dispersions;
            QVector<float> sigmoids;

            for ( int i=0; i<ELECTRODES_AMOUNT; i++ ){

                qCDebug(RCG) << Q_FUNC_INFO << "channel#" << i << gestData.m_channel[i];

                float AVG = average( gestData.m_channel[i] );
                float DISP = dispersion( gestData.m_channel[i], AVG );
                float SIGMA = sigma(DISP, m_K , m_r, m_beta );

                averages    << AVG;
                dispersions << DISP;
                sigmoids    << SIGMA;
            }
            qCDebug(RCG) << Q_FUNC_INFO << "AVERAGE" << averages;
            qCDebug(RCG) << Q_FUNC_INFO << "DISPERS" << dispersions;
            qCDebug(RCG) << Q_FUNC_INFO << "SIGMOID" << sigmoids;

            if ( 8 == sigmoids.size() ){

                float result[4]    = {0, 0, 0, 0};
                float vect[8]      = {sigmoids.at(0), sigmoids.at(1), sigmoids.at(2), sigmoids.at(3), sigmoids.at(4), sigmoids.at(5), sigmoids.at(6), sigmoids.at(7)};

                multiplication( m_matrix, vect, result );
                QVector<float> data;
                data << result[0] << result[1] << result[2] <<  result[3];
                m_cls.Add( data, gestData.m_gest );
            }
            else {
                m_error = QString::fromUtf8("bad sigmoids size!");
                qCCritical(RCG) << Q_FUNC_INFO << m_error;
                m_bError = true;
                res = false;
            }
            qCDebug(RCG) << Q_FUNC_INFO << "-------------------------------------------------------";
        }
    }
    else
        qCCritical(RCG) << Q_FUNC_INFO << "matrix loading error!";

    return (res);
}

void Recognizer::_start(){
    qCDebug(RCG) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();
    if ( !m_bEnabled )
        m_bEnabled = true;
}

void Recognizer::_stop(){
    qCDebug(RCG) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();
    if ( m_bEnabled )
        m_bEnabled = false;
}


//////////////////////////////////////////////////////////////////////
/// контроллер
Controller::Controller(){
    qCDebug(RCG) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();
}

Controller::~Controller(){
    qCDebug(RCG) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();
    stop();
}

bool Controller::start(const int window, const int shift, const float K, const float r, const float Beta, const int queue, QVector<GestData> gestsDB){
    if ( !m_enable ){
        m_enable = true;

        Recognizer * pRcgn = new Recognizer( window, shift, K, r, Beta, queue );
        pRcgn->load( gestsDB );
        pRcgn->moveToThread(&m_thread);

        connect(&m_thread,  &QThread::finished,     pRcgn,  &QObject::deleteLater );
        connect(&m_thread,  &QThread::started,      pRcgn,  &Recognizer::_start );
        connect( this,      &Controller::s_packet,  pRcgn,  &Recognizer::_packet );
        connect( pRcgn,     &Recognizer::s_gest,    this,   &Controller::_gest );

        m_thread.start();
        do{
            QThread::msleep(1);
            QThread::yieldCurrentThread();

        } while ( !m_thread.isRunning() );
        return (true);
    }
    m_error = QString::fromUtf8("Распознаватель уже запущен!");
    return(false);
}

bool Controller::start(const int window, const int shift, const float K, const float r, const float Beta, const int queue, QVector<GestData> gestsDB, QString path){
    if ( !m_enable ){
        m_enable = true;

        Recognizer * pRcgn = new Recognizer( window, shift, K, r, Beta, queue );
        bool res = pRcgn->load( path, gestsDB );
        if ( res ) {
            pRcgn->moveToThread(&m_thread);

            connect(&m_thread,  &QThread::finished,     pRcgn,  &QObject::deleteLater );
            connect(&m_thread,  &QThread::started,      pRcgn,  &Recognizer::_start );
            connect( this,      &Controller::s_packet,  pRcgn,  &Recognizer::_packet );
            connect( pRcgn,     &Recognizer::s_gest,    this,   &Controller::_gest );

            m_thread.start();
            do{
                QThread::msleep(1);
                QThread::yieldCurrentThread();

            } while ( !m_thread.isRunning() );
            return(true);
        }
        else {
            m_error = pRcgn->getError();
            return(false);
        }
    }

    m_error = QString::fromUtf8("Распознаватель уже запущен!");
    return(false);
}

void Controller::stop(){
    if ( m_enable ){
        m_thread.quit();
        m_thread.wait();
        m_enable = false;
    }
}

void Controller::_packet( MCU::SDU_MYOGRAPH_DATA packet ){
    if ( m_enable ){
        emit s_packet( packet );
    }
}

void Controller::_gest(int gest){
    if ( m_enable ){
        emit s_gest( gest );
    }
}
}//MATH
