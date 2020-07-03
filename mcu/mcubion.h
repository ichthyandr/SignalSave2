#ifndef MCUBION_H
#define MCUBION_H

#include "packets.h"
#include <QByteArray>
#include <QSerialPort>
#include <QThread>
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(BION)

namespace MCUBION {
/// воркер
class serialWorker: public QObject {
    Q_OBJECT
private:
    /// флаг - признак
    bool        m_bEnabled;
    /// наименование порта
    QString     m_portName;
    /// скорость обмена
    qint32      m_baudRate;
    /// посл. порт
    QSerialPort m_serial;
    /// максимальный размер буфера
    const int   m_buffMAX{4096};
    /// буфер принятых данных
    QByteArray  m_buffer;
    /// количество обнаруженных PDU
    quint32     m_pdu_counter{0};
    /// количество PDU c битым crc16
    quint32     m_crc_err_counter{0};
    /// найти PDU в буффере и извлечь SDU
    /// \return SDU, иначе длина 0
    QByteArray take_SDU();
public:
    /// конструктор класса
    ///\public
    serialWorker( const QString & portName, const qint32 baudRate, QObject *parent = Q_NULLPTR );
    /// деструктор класса
    ///\public
    ~serialWorker();
public slots:
    /// получить данные из порта
    void _get();
    /// запустить поток управления
    void _on();
    /// выключить поток управления
    void _off();
signals:
    /// сигнализировать принятое сообщение
    ///\param msg[in] - принятое сообщение
    void s_recv( const QByteArray & msg );
    /// сигнализировать состояние порта
    ///\param msg[in] - статус
    void s_status( const QString & msg );
};


/// контроллер
class Controller: public QObject {
    Q_OBJECT
private:
    /// коэфф усиления, по умолчанию 1
    int m_gain{1};
    /// признак работы
    bool    m_enable{false};
    /// рабочий поток
    QThread m_thread;
signals:
    /// сигнализировать состояние воркера наверх
    /// \param msg[in] - состояние
    void s_state (const QString & msg );
    /// передать в GUI декодированный SDU в режиме MYOGRAPH
    /// \param sdu[in] - декодированный sdu
    void s_myograph_data( MCU::SDU_MYOGRAPH_DATA sdu );
public:
    /// конструктор класса
    ///\public
    Controller( QObject *parent = Q_NULLPTR );
    /// деструктор класса
    ///\public
    virtual ~Controller();
    /// запустить управление
    ///\param portName  - имя порта
    ///\param baudRate  - скорость обмена
    ///\param gain      - коэфф. усиления
    void on( const QString & portName, const qint32 baudRate, const int gain = 1 );
    /// отключить управление
    void off();
    /// \brief возвращает состояние контроллера
    /// \return true - в работе
    bool isEnabled() const;
public slots:
    /// принять сообщение из воркера для декодирования
    ///\param cmd[in] - команда
    void _get( const QByteArray & msg );
    /// принять состояние воркера
    /// \param msg[in] - состояние
    void _status( const QString & msg );
};

}

#endif // MCUBION_H
