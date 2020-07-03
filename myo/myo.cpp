/*utf8*/
#include "myo.h"

Q_LOGGING_CATEGORY(MYODBG,  "MYODBG")

class DataCollector : public myo::DeviceListener {

public:
    DataCollector(): emgSamples(){}

    void onPair	(myo::Myo * myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion){
        Q_UNUSED( myo );
        Q_UNUSED( timestamp );
        Q_UNUSED( firmwareVersion );
        m_state = MYO::Pair;
    }

    // onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
    void onUnpair(myo::Myo* myo, uint64_t timestamp) {
        Q_UNUSED( myo );
        Q_UNUSED( timestamp );
        // We've lost a Myo.
        // Let's clean up some leftover state.
        emgSamples.fill(0);
        m_state = MYO::Unpair;
    }

    void onConnect(myo::Myo* myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion) {
        Q_UNUSED(myo);
        Q_UNUSED(timestamp);
        Q_UNUSED(firmwareVersion);

        //emit myMyo->s_connect();
        myo->setStreamEmg(myo::Myo::streamEmgEnabled);
        m_state = MYO::Connect;
    }

    /// Called when a paired Myo has been disconnected.
    void onDisconnect(myo::Myo* myo, uint64_t timestamp) {
        Q_UNUSED(myo)
        Q_UNUSED(timestamp)

        //emit myMyo->s_disconnect();
        emgSamples.fill(0);
        m_state = MYO::Disconnect;
    }

    void onArmUnsync(myo::Myo* myo, uint64_t timestamp) {
        Q_UNUSED(myo)
        Q_UNUSED(timestamp)

        m_state = MYO::ArmUnsync;
    }

    void onArmSync(	myo::Myo * myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation, myo::WarmupState warmupState){
        Q_UNUSED(myo);
        Q_UNUSED(timestamp);
        Q_UNUSED(arm);
        Q_UNUSED(xDirection);
        Q_UNUSED(rotation);
        Q_UNUSED(warmupState);

        m_state = MYO::ArmSync;
    }

    void onLock( myo::Myo * myo, uint64_t timestamp ){
        Q_UNUSED(myo);
        Q_UNUSED(timestamp);

        m_state = MYO::Lock;
    }

    void onUnlock( myo::Myo * myo, uint64_t timestamp ){
        Q_UNUSED(myo);
        Q_UNUSED(timestamp);

        m_state = MYO::Unlock;
    }

    void onWarmupCompleted ( myo::Myo * myo, uint64_t timestamp, myo::WarmupResult warmupResult){
        Q_UNUSED(myo);
        Q_UNUSED(timestamp);
        Q_UNUSED(warmupResult);

        m_state = MYO::WarmupCompleted;
    }

    // onEmgData() is called whenever a paired Myo has provided new EMG data, and EMG streaming is enabled.
    void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg){
        Q_UNUSED( myo );
        Q_UNUSED( timestamp );
        for (int i = 0; i < 8; i++) {
            emgSamples[i] = emg[i];
        }
    }


    /// The values of this array is set by onEmgData() above.
    std::array<int8_t, 8>   emgSamples;
    /// Myo State
    MYO::state_t            m_state{MYO::Undef};

};

DataCollector g_collector;


//////////////////////////////////////////////////////////////////////
/// Worker
///

MYO::Worker::Worker():
    /*m_hub("com.example.emg-data-sample"),*/
    m_pCollector(&g_collector){
    qCDebug(MYODBG) << Q_FUNC_INFO << "thread ID=" << QThread::currentThreadId();

    try {
        m_pHub = new myo::Hub("com.example.emg-data-sample");
    }
    catch ( const std::exception& e ){
        qCCritical(MYODBG) << Q_FUNC_INFO << e.what();
        throw e;
        return;
    }
    catch ( ... ){
        qCCritical(MYODBG) << Q_FUNC_INFO << "undef exception";
        std::exception e("MYO::Worker::Worker undefined exception!");
        throw e;
        return;
    }

    connect( this, &MYO::Worker::s_get, this, &MYO::Worker::get, Qt::QueuedConnection );
}

MYO::Worker::~Worker(){
    qCDebug(MYODBG) << Q_FUNC_INFO << "thread ID=" << QThread::currentThreadId();
    if ( 0 != m_pHub ) delete m_pHub;
}


void MYO::Worker::on(){
    qCDebug(MYODBG) << Q_FUNC_INFO << "thread ID=" << QThread::currentThreadId() << "m_enable=" << m_enable;
    if ( !m_enable ){
        m_enable = true;

        try {

            // Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
            // immediately.
            // waitForMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
            // if that fails, the function will return a null pointer.

            //m_pMyo = m_hub.waitForMyo(10000);
            m_pMyo = m_pHub->waitForMyo(10000);
            if (!m_pMyo) {
                m_enable = false;
                qCCritical(MYODBG) << Q_FUNC_INFO << "Unable to find a Myo!";
                emit s_error( QString::fromUtf8("Myo не найден!") );
                return;
            }

            // We've found a Myo.
            qCDebug(MYODBG) << Q_FUNC_INFO << "Connected to a Myo armband!";
            //emit s_state(QString::fromUtf8("Myo подключен"));

            // Next we enable EMG streaming on the found Myo.
            m_pMyo->setStreamEmg(myo::Myo::streamEmgEnabled);

            // Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
            // Hub::run() to send events to all registered device listeners.

            //m_hub.addListener( m_pCollector);
            m_pHub->addListener( m_pCollector);

        }
        catch (const std::exception& e) {
            m_enable = false;
            qCCritical(MYODBG) << Q_FUNC_INFO << e.what();
            emit s_error( QString(e.what()) );
            return;
        }
        // получить данные из Myo
        get();
    }
    else
        qCDebug(MYODBG) << Q_FUNC_INFO << "already enabled!";
}

void MYO::Worker::off(){
    qCDebug(MYODBG) << Q_FUNC_INFO << "thread ID=" << QThread::currentThreadId() << "m_enable=" << m_enable;
    if ( m_enable ){
        m_enable = false;
    }
    else
        qCDebug(MYODBG) << Q_FUNC_INFO << "already disabled!";
}

void MYO::Worker::get(){
    if ( m_enable ){
        try {
            //m_hub.run(4);
            m_pHub->run(4);

            QDateTime   date_time   = QDateTime::currentDateTime ();
            qint64      stampms     = date_time.toMSecsSinceEpoch();

            float       s0          = m_pCollector->emgSamples[0];
            float       s1          = m_pCollector->emgSamples[1];
            float       s2          = m_pCollector->emgSamples[2];
            float       s3          = m_pCollector->emgSamples[3];
            float       s4          = m_pCollector->emgSamples[4];
            float       s5          = m_pCollector->emgSamples[5];
            float       s6          = m_pCollector->emgSamples[6];
            float       s7          = m_pCollector->emgSamples[7];
/*
            qCDebug(MYOwrk) << Q_FUNC_INFO << stampms << s0 << s1 << s2 << s3 << s4 << s5 << s6 << s7
                            << "thread ID=" << QThread::currentThreadId()
                            << "state=" << m_pCollector->m_state;
*/
            emit s_data( stampms, s0, s1, s2, s3, s4, s5, s6, s7 );

            if ( m_lastState != m_pCollector->m_state ){
                m_lastState = m_pCollector->m_state;
                emit s_state( m_pCollector->m_state );
            }

            emit s_get();
        }
        catch (const std::exception& e){
            m_enable = false;
            qCCritical(MYODBG) << Q_FUNC_INFO << e.what();
            emit s_error( QString(e.what()) );
        }
    }
}


//////////////////////////////////////////////////////////////////////
/// Controller
///

MYO::Controller::Controller(){
    qCDebug(MYODBG) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();
}

MYO::Controller::~Controller(){
    qCDebug(MYODBG) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();
    off();
}

bool MYO::Controller::on(int gain){
    qCDebug(MYODBG) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();

    if ( !m_enable ){
        m_lastStampms = -1;
        m_N           = 0;
        m_gain        = gain;

        qCDebug(MYODBG) << Q_FUNC_INFO << "m_gain=" << m_gain;

        MYO::Worker * pWorker = 0;
        try {
            pWorker = new MYO::Worker( );
            m_enable = true;
        }
        catch ( const std::exception& e ){
            qCCritical(MYODBG) << Q_FUNC_INFO << e.what();
            error( e.what() );
            return (false);
        }
        catch ( ... ){
            qCCritical(MYODBG) << Q_FUNC_INFO << "undef exception";
            error( "undef exception" );
            return (false);
        }

        pWorker->moveToThread(&m_thread);

        connect(&m_thread,  &QThread::finished,         pWorker,    &QObject::deleteLater );
        connect(&m_thread,  &QThread::started,          pWorker,    &MYO::Worker::on );
        connect(pWorker,    &MYO::Worker::s_data,       this,       &MYO::Controller::data );
        connect(pWorker,    &MYO::Worker::s_state,      this,       &MYO::Controller::state );
        connect(pWorker,    &MYO::Worker::s_error,      this,       &MYO::Controller::error );
        connect(this,       &MYO::Controller::s_off,    pWorker,    &MYO::Worker::off);

        m_thread.start();

        do {
            QThread::msleep(1);
            QThread::yieldCurrentThread();

        } while ( !m_thread.isRunning() );
    }
    return (true );
}

void MYO::Controller::off(){
    qCDebug(MYODBG) << Q_FUNC_INFO << " thread ID=" << QThread::currentThreadId();
    if ( m_enable ){
        m_enable = false;
        emit s_off();
        m_thread.quit();
        m_thread.wait();
    }
}

void MYO::Controller::data( qint64 stampms, float s0, float s1, float s2, float s3, float s4, float s5, float s6, float s7 ){

    if ( m_enable ) {
        qCDebug(MYODBG) << Q_FUNC_INFO << stampms << s0 << s1 << s2 << s3 << s4 << s5 << s6 << s7;
        MCU::SDU_MYOGRAPH_DATA sdu;

        sdu.m_F = 200;  // 200Гц

        sdu.m_data[0] = s0 * m_gain;
        sdu.m_data[1] = s1 * m_gain;
        sdu.m_data[2] = s2 * m_gain;
        sdu.m_data[3] = s3 * m_gain;
        sdu.m_data[4] = s4 * m_gain;
        sdu.m_data[5] = s5 * m_gain;
        sdu.m_data[6] = s6 * m_gain;
        sdu.m_data[7] = s7 * m_gain;

        /*if ( m_N > 100 ) { error("error"); return; }*/

        if ( m_lastStampms < 0 ){
            m_lastStampms = stampms;
            sdu.m_N = m_N;
            emit s_myograph_data( sdu );
            qCDebug(MYODBG) << Q_FUNC_INFO << "m_lastStampms<0" << sdu;
        }
        else {
            qint64 delta = stampms - m_lastStampms;
            m_lastStampms = stampms;
            int amount = delta / 5; // пакеты идут с периодом 5мс, если была потеря данных, то отправляем столько пакетов, сколько было потеряно
            if ( (delta % 5) > 3 ) amount++;
            if ( 0 == amount ){
                m_N++;
                sdu.m_N = m_N;
                emit s_myograph_data( sdu );
                qCDebug(MYODBG) << Q_FUNC_INFO << "delta=" << delta << "0==amount" << sdu;
            }
            else for (int i = 0; i < amount; i++ ){
                m_N++;
                sdu.m_N = m_N;
                emit s_myograph_data( sdu );
                qCDebug(MYODBG) << Q_FUNC_INFO << "delta=" << delta << "i=" << i << sdu;
            }
        }
    }
}

void MYO::Controller::state(int state ){
    qCDebug(MYODBG) << Q_FUNC_INFO << state;
    emit s_state( state );
}

void MYO::Controller::error( const QString & msg ){
    qCDebug(MYODBG) << Q_FUNC_INFO << msg;
    off();
    emit s_error( msg );
}
