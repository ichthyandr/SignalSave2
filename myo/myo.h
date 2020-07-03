/*utf8*/
#ifndef MYO_H
#define MYO_H
#include <QObject>

#pragma warning(push)
#pragma warning(disable:4100)
#pragma warning(disable:4297)
#include <myo/myo.hpp>
#pragma warning(pop)

#include <QDateTime>
#include <QThread>
#include <array>
#include "../mcu/packets.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(MYODBG)

class DataCollector;

namespace MYO {

typedef enum _state{
    Undef           = 0,
    Connect         = 1,
    Disconnect      = 2,
    ArmSync         = 3,
    ArmUnsync       = 4,
    Lock            = 5,
    Unlock          = 6,
    Pair            = 7,
    Unpair          = 8,
    WarmupCompleted = 9
} state_t;

/// класс воркера MYO
class Worker:  public QObject {
    Q_OBJECT

public:
    /// конструктор
    Worker();
    /// деструктор
    ~Worker();

public slots:
    /// включить воркер
    void on();
    /// выключить воркер
    void off();

private slots:
    /// получить данные из MYO (DataCollector) и передать контроллеру
    void get();

signals:
    /// сигнал самому себе, для циклического вызова void get();
    void s_get();
    /// сигнализировать фатальную ошибку в контроллер
    /// \param msg[in] - текст ошибки
    void s_error (const QString & msg );
    /// сигнализировать состояние воркера в контроллер
    ///\param state - состояние MYO
    void s_state ( int state );
    /// передать в контроллер данные браслета
    ///\param stampms   - таймстамп (мс) с начала эпохи
    ///\param s0..s7    - отсчеты с датчиков
    void s_data( qint64 stampms, float s0, float s1, float s2, float s3, float s4, float s5, float s6, float s7 );
private:

    /// признак работы
    bool            m_enable{false};
    /// Myo
    myo::Myo *      m_pMyo{0};
    /// Myo Hub
    myo::Hub *      m_pHub{0};
    //myo::Hub       m_hub;
    /// DataCollector
    DataCollector * m_pCollector;
    /// состояние
    state_t         m_lastState{Undef};
};

/// класс контроллера MYO
class Controller: public QObject {
    Q_OBJECT
private:
    /// рабочий поток
    QThread     m_thread;
    /// признак работы
    bool        m_enable{false};
    /// последний таймстамп
    qint64      m_lastStampms{-1};
    /// счетчик пакетов
    uint16_t    m_N{0};
    /// усиление
    int         m_gain{1};
public:
    /// конструктор
    Controller();
    /// деструктор
    ~Controller();
private slots:
    /// принять из воркера данные браслета
    ///\param stampms   - таймстамп (мс) с начала эпохи
    ///\param s0..s7    - отсчеты с датчиков
    void data( qint64 stampms, float s0, float s1, float s2, float s3, float s4, float s5, float s6, float s7 );
    /// принять из воркера состояние
    ///\param state - состояние MYO
    void state( int state );
    /// принять из воркера ошибку
    /// \param msg[in] - текст ошибки
    void error( const QString & msg );
public slots:
    /// запустить MYO из GUI
    bool on( int gain = 1);
    /// отключить MYO из GUI
    void off();
signals:
    /// остановить воркер
    void s_off();
    /// сигнализировать ошибку в GUI
    /// \param msg[in] - текст ошибки
    void s_error (const QString & msg );
    /// сигнализировать состояние в GUI
    ///\param state - состояние MYO
    void s_state ( int state );
    /// передать в GUI декодированный SDU в режиме MYOGRAPH
    /// \param sdu[in] - декодированный sdu
    void s_myograph_data( MCU::SDU_MYOGRAPH_DATA sdu );
};

}

#endif // MYO_H
