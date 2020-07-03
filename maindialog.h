/*utf8*/
#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include "mcu/packets.h"
#include "mcu/mcubion.h"
#include "myo/myo.h"
#include "math/recognizer.h"

#include <QDialog>
#include <QString>
#include <QProgressBar>
#include <QTimer>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(GUI)

/// таймаут 2с, если нет пакетов от браслета в режиме записи
const int TIMEOUT           = 2000;

/// Основное окно
class MainDialog : public QDialog {
    Q_OBJECT
private:
    /// допустимые состояния
    enum LRN_STATE {
        LRN_NODATA      = 1,    // начальное состояние, данных нет
        LRN_READY       = 2,    // состояние готовности для записи данных
        LRN_RECORDING   = 3,    // запись данных
        LRN_DONE        = 4,    // данные записаны и готовы для расчета и загрузки
        LRN_TESTING     = 5     // тестирование
    };
    /// состояние виджета, изначально данных нет
    LRN_STATE m_state{LRN_NODATA};
    /// изменить состояние элементов управления согласно состоянию виджета
    void update();
    /// отобразить жест
    /// \param gest[in] - номер жеста от 1 до GESTURES_AMOUNT включительно
    /// \param repeat[in] - номер повтора записи
    void showGest(const int gest , int repeat = 0);
    /// отобразить транспарант - "Данных нет"
    void showNoData();
    /// отобразить транспарант - "Данные записаны"
    void showDone();
    /// параметры обучения
    struct Params {
        /// "усиление" отсчетов, по умолчанию 1
        int m_gain{1};
        /// количество повторов (векторов)
        int m_vectors{5};
        /// ширина окна в количестве выборок
        int m_window{DEFAULT_SLWINDOW_SIZE};
        /// сдвиг окна в количестве выборок
        int m_shift{DEFAULT_SLWINDOW_SHIFT};
        /// кэффициент К
        int m_K{DEFAULT_K};
        /// кэффициент r
        int m_r{DEFAULT_R};
        /// кэффициент Beta
        double m_Beta{DEFAULT_BETA};
        /// глубина очереди гистограммы
        int m_histogram_queue{DEFAULT_HISTOGRAM_QUEUE};
        /// имя файла матрицы
        QString m_matrix{""};
        /// флажок левой руки
        bool m_leftHand{true};
        /// массив флагов обучаемых жестов
        /// true - жест нужно записать
        /// false - не записывать
        QVector<bool> m_gests{QVector<bool>(GESTURES_AMOUNT,false)};
    } m_params;
    /// вектор адресов виджетов-параметров
    QVector<QWidget*> m_paramsGroup;
    /// вектор адресов меток сенсоров
    QVector<QLabel*> m_sensors;
    /// номера векторов для обучения
    QVector<int> m_gests;
    /// текущий номер повторений
    int m_repeat{1};
    /// текущий записываемый жест
    GestData m_gestData;
    /// данные по всем записаным жестам
    QVector<GestData> m_gestsDB;
    /// таймер таймаута при записи данных
    QTimer * m_pTimer{Q_NULLPTR};
    /// Прогресс бар
    QProgressBar * m_pProgress{Q_NULLPTR};
    /// Список картинок жестов
    QList<QImage> m_listImages;

public:
    /// Конструктор
    MainDialog(QWidget *parent = 0);
    /// Деструктор
    ~MainDialog();
private:
    /// печать статуса
    void printStatus( const QString & status );
private slots:
    /// слот обработки чекбокса жеста
    /// \param checked[in] - состояние чекбокса
    void _gestChBx1(bool checked);
    /// слот обработки чекбокса жеста
    /// \param checked[in] - состояние чекбокса
    void _gestChBx2(bool checked);
    /// слот обработки чекбокса жеста
    /// \param checked[in] - состояние чекбокса
    void _gestChBx3(bool checked);
    /// слот обработки чекбокса жеста
    /// \param checked[in] - состояние чекбокса
    void _gestChBx4(bool checked);
    /// слот обработки чекбокса жеста
    /// \param checked[in] - состояние чекбокса
    void _gestChBx5(bool checked);
    /// слот обработки чекбокса жеста
    /// \param checked[in] - состояние чекбокса
    void _gestChBx6(bool checked);
    /// слот обработки чекбокса жеста
    /// \param checked[in] - состояние чекбокса
    void _gestChBx7(bool checked);
    /// слот обработки чекбокса жеста
    /// \param checked[in] - состояние чекбокса
    void _gestChBx8(bool checked);
    /// слот обработки чекбокса жеста
    /// \param checked[in] - состояние чекбокса
    void _gestChBx9(bool checked);
    /// слот обработки чекбокса жеста
    /// \param checked[in] - состояние чекбокса
    void _gestChBx10(bool checked);
    /// слот обработки чекбокса жеста
    /// \param checked[in] - состояние чекбокса
    void _gestChBx11(bool checked);
    /// слот обработки чекбокса жеста
    /// \param checked[in] - состояние чекбокса
    void _gestChBx12(bool checked);
    /// слот обработки чекбокса жеста
    /// \param checked[in] - состояние чекбокса
    void _gestChBx13(bool checked);
    /// слот обработки чекбокса жеста
    /// \param checked[in] - состояние чекбокса
    void _gestChBx14(bool checked);

    /// оброботка спинбокса - Усиление
    ///\param value[in] - Коэфф усиления
    void _gain( int value );
    /// оброботка спинбокса - Количество повторов (векторов)
    ///\param value[in] - Количество повторов (векторов)
    void _vectors( int value );
    /// оброботка спинбокса - Ширина окна в количестве выборок
    ///\param value[in] - Ширина окна в количестве выборок
    void _window( int value );
    /// оброботка спинбокса - Сдвиг окна в количестве выборок
    ///\param value[in] - Сдвиг окна в количестве выборок
    void _shift( int value );
    /// оброботка спинбокса - коэффициент К
    ///\param value[in] - коэффициент К
    void _K( int value );
    /// оброботка спинбокса - коэффициент r
    ///\param value[in] - коэффициент r
    void _r( int value );
    /// оброботка спинбокса - коэффициент Beta
    ///\param value[in] - коэффициент Beta
    void _Beta( double value );
    /// оброботка спинбокса - размер очереди гистограммы
    ///\param value[in] - коэффициент r
    void _histogram( int value );
    /// обработка строки ввода файла матрицы
    void _matrix(const QString &text);
    /// обработка кнопки "Матрица"
    void _file();
    /// оброботка комбобокса - коэффициент Рука
    ///\param index[in] - селектированный индекс
    void _hand(int index);
    /// обработка кнопки "Старт"
    void _start( );
    /// обработка кнопки "Запись"
    void _record( );
    /// обработка кнопки "Сброс"
    void _reset( );
#if 0
    /// обработка кнопки "Загрузка"
    void _load( );
#endif
    /// обработка кнопки "Тест"
    void _test( );
    /// обработка кнопки "Стоп" тестирования
    void _stopTst();
    /// начать запись
    void _startRecord();
    /// остановить запись
    void _stopRecord();
    /// таймаут при записи данных
    void _timeout();
    /// сохранить запись
    void _save();


    /// слот приема входящего пакета
    ///\param packet[in] - входящий декодированный myo пакет
    void _packet( MCU::SDU_MYOGRAPH_DATA packet );
    /// слот приема распознанного жеста
    ///\param gest[in] - распознанный жест
    void _gest(int gest);

signals:
    /// начать запись
    void s_startRecord();
    /// остановить запись
    void s_stopRecord();

private slots: // for myo
    /// подключить MYO
    void _connectMYO();
    /// отключить MYO
    void _disconnectMYO();
    /// состояние MYO
    ///\param state[in] - состояние
    void _stateMYO( int state );
    /// ошибка MYO
    ///\param msg[in] - текст ошибки
    void _errorMYO( const QString & msg );

private slots: // for mcu
    /// подключить MCU
    void _connectMCU();
    /// отключить MCU
    void _disconnectMCU();
    /// принять состояние MCU
    /// \param msg[in] - состояние
    void _state (const QString & msg );

private: // хендлеры
    /// хендлер MCU
    MCUBION::Controller m_mcu;
    /// хендлер myo
    MYO::Controller     m_myo;
    /// хендлер рекогнайзера
    MATH::Controller    m_rcgn;
};

/// виджет порта
/// используется управлением браслета
class portWidget: public QWidget {
    Q_OBJECT
public:
    /// конструктор
    ///\public
    ///\param objectName[in] - object name
    ///\param parent[in] - родитель окна
    ///\param bMyo[in] - флаг для Myo
    portWidget( QString objectName, QWidget * parent = Q_NULLPTR, bool bMyo = false );
    /// деструктор
    ///\public
    virtual ~portWidget();
    /// вернуть имя порта
    ///\return имя порта
    QString getPortName();
    /// вернуть скорость передачи
    ///\return скорость передачи
    qint32 getBaudRate();
    /// перевести виджет в состояние "Выключено"
    void off();
    /// запретить Myo
    void disableMyo();
    /// разрешить Myo
    void enableMyo();
private:
    /// используется Myo
    bool    m_bMyo{false};
    /// включено/выключено
    bool    m_enabled{false};
    /// скорость обмена
    qint32  m_baudRate{0};
    /// название com порта
    QString m_portName{""};
    /// кнопка Вкл\Выкл
    QPushButton * m_onoffBtn{Q_NULLPTR};
    /// порт комбо
    QComboBox * m_portCombo{Q_NULLPTR};
    /// скорость передачи комбо
    QComboBox * m_speedCombo{Q_NULLPTR};
    /// чекбокс Myo
    QCheckBox * m_myoCheckBox{Q_NULLPTR};
private slots:
    /// слот кнопки
    void OnOff();
    /// слот выбора порта
    ///\param port[in] - порт
    void portChanged(const QString & port );
    /// слот выбора скорости
    ///\param speed[in] - скорость
    void speedChanged( const QString & speed );
    /// слот чекбокса Myo
    ///\param state[in] - состояние
    void chkbxChanged(int state);
signals:
    /// сигнал Включить порт
    void s_on();
    /// сигнал Выключить порт
    void s_off();
signals: /*сигналы только для Myo браслета*/
    /// включить Myo
    void s_onMyo();
    /// выключить Myo
    void s_offMyo();
};


#endif // MAINDIALOG_H
