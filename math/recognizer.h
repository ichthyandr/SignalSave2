/*utf8*/
#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include "mcu/packets.h"

#include <QObject>
#include <QVector>
#include <QQueue>
#include <QThread>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(RCG)
/// длина скользящего окна по умолчанию
#define DEFAULT_SLWINDOW_SIZE       100
/// размер смещения скользящего окна по умолчанию
#define DEFAULT_SLWINDOW_SHIFT      5
/// значение коэфф. К при вычислении сигмоиды по умолчанию
#define DEFAULT_K                   200000
/// значение коэфф. r при вычислении сигмоиды по умолчанию
#define DEFAULT_R                   2
/// значение коэфф. Beta при вычислении сигмоиды по умолчанию
#define DEFAULT_BETA                4.00
/// общее количество жестов
#define GESTURES_AMOUNT             14
/// длина очереди гистограммы по умолчанию
#define DEFAULT_HISTOGRAM_QUEUE     20
/// кол-во электродов на браслете
#define ELECTRODES_AMOUNT           8

/// структура записанных данных жеста
struct GestData{
    /// счетчик записей в m_channel
    int m_counter{0};
    /// номер жеста (класс)
    int m_gest{0};
    /// номер повтора или номер вектора
    int m_number{0};
    /// массив записанных данных датчиков браслета
    QVector<float>  m_channel[ELECTRODES_AMOUNT];
};

namespace MATH {
/// класс рекогнайзера
class Recognizer: public QObject{
    Q_OBJECT
private:
    /// класс скользящего окна
    /// используется для формирования фреймов при вычислении огибающей
    class slidingWindow {
    private:
        /// размер окна, значение по умолчанию
        int m_window{DEFAULT_SLWINDOW_SIZE};
        /// сдвиг окна, значение по умолчанию
        int m_shift{DEFAULT_SLWINDOW_SHIFT};
        /// буфер мио пакетов от браслета
        QVector<MCU::SDU_MYOGRAPH_DATA> m_buffer;
    public:
        /// конструктор
        slidingWindow();
        /// деструктор
        ~slidingWindow();
        /// установить параметры окна, перед установкой параметров буфер очищается
        /// \param window[in] - размер окна, должен быть больше значения shift, иначе используются значения по умолчанию
        /// \param shift[in] - смещение окна, должно быть больше нуля и меньше размера окна, иначе используются значения по умолчанию
        void setUp( const int window, const int shift );
        /// записать в буфер входящий пакет
        /// \param packet[in] - входящий декодированный myo пакет
        /// \param frame[out] - готовый фрейм для обработки
        /// \return true, если фрейм готов для обработки, false - фрейм не готов
        bool packet( MCU::SDU_MYOGRAPH_DATA packet, QVector<MCU::SDU_MYOGRAPH_DATA> & frame );
    };

    ///Представление класса (хвата)
    struct Class{
        ///вектор огибающей
        QVector<float> m_data;
        ///номер жеста, жесты нумеруются с 1
        int m_gest{0};
        ///Конструтор
        ///\param data[in] - вектор огибающей
        ///\param gest[in] - номер жеста 1 ...
        Class( QVector<float> & data, int gest );
        ///Конструктор по умолчанию
        Class(){}
    };

    ///Классификатор
    class Classificator{
        /// список классов
        QVector<Class> m_classes;
    public:
        ///Добавить класс
        ///\param data[in] - вектор данных
        ///\param gest[in] - номер класса(хват)
        void Add(QVector<float> & data, int gest);
        /// Распознать вектор
        ///\param vector[in] - входной вектор
        ///\return распознанный номер жеста
        int Recognize(QVector<float> & vect);
        ///Очистить классификатор
        void clear();
    };

    /// класс гистограммы
    class Histogram{
        /// Массив весов жестов
        int m_frequency[GESTURES_AMOUNT];
        /// Макс глубина очереди.
        int m_max{DEFAULT_HISTOGRAM_QUEUE};
        /// очередь гистограммы
        QQueue<int> m_queue;
    public:
        /// Коструктор
        /// \param max[in] - размер очереди гистограммы
        Histogram( const int max = DEFAULT_HISTOGRAM_QUEUE );
        /// установить размер окна гистограммы, все значения обнуляются
        /// \param queue[in] - размер очереди гистограммы, должен быть больше 0
        void setUp( const int queue );
        /// возвращает номер жеста с максимальным весом
        /// \param gest[in] - новый номер жеста, поступающий в "окно" гистограммы, номера жестов от 1 до GESTURES_AMOUNT включительно
        /// \return номер жеста с максимальным весом от 1 до GESTURES_AMOUNT включительно или -1 в случае ошибки
        int weigh(int gest);
    };

private:
    /// определение типа распознавателя
    typedef enum {
        NEIGHBOR,
        HASH
    } NNType_t;
    /// тип распознавателя
    NNType_t        m_type{NEIGHBOR};
    /// хендлер скользящего окна
    slidingWindow   m_slwin;
    /// хендлер классификатора
    Classificator   m_cls;
    /// хендлер гистограммы
    Histogram       m_histogram;
    /// коэфф К
    float m_K{DEFAULT_K};
    /// коэфф r
    float m_r{DEFAULT_R};
    /// коэф beta
    float m_beta{DEFAULT_BETA};
    /// признак работы
    bool m_bEnabled{false};

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /// myo left matrix by default FOR HASH
    float m_matrix[4][8]={
        {-0.06002f,  -0.64518f,   -0.2355f,    0.0955f,     0.06781f,    -0.4512f,    -0.23421f,   -0.6453f},
        {-0.61502f,  -0.04632f,   0.13065f,    0.47059f,    -0.49964f,   -0.59558f,   0.20971f,    -0.02886f},
        {0.26795f,   0.16076f,    0.21494f,    0.2452f,     0.00975f,    0.22505f,    0.23558f,    0.16408f},
        {0.33092f,   0.12086f,    0.08813f,    0.25776f,    0.04172f,    -0.18364f,   -0.12095f,   -0.24235f}
    };
    /// признак ошибки загрузки матрицы FOR HASH
    bool m_bError{false};
    /// текст ошибки при неудачной загрузке матрицы
    QString m_error{""};

    /// установить параметры рекогнайзера
    ///\param window[in]    - размер окна, должен быть больше значения shift, иначе используются значения по умолчанию
    ///\param shift[in]     - смещение окна, должно быть больше нуля и меньше размера окна, иначе используются значения по умолчанию
    ///\param K[in]         - коэфф. К для расчета сигмоиды
    ///\param r[in]         - коэфф. r для расчета сигмоиды
    ///\param Beta[in]      - коэфф. Beta для расчета сигмоиды
    ///\param queue[in]     - размер очереди гистограммы, должен быть больше 0
    void setUp( const int window, const int shift, const float K, const float r, const float Beta, const int queue );

    /// загрузить хеш-матрицу, если будет использоваться хеширование
    ///\param path[in] - имя файла матрицы
    ///\return true - матрица загружена, false - есть ошибка
    bool loadMatrix( QString path );

public:
    /// конструктор
    Recognizer();
    /// конструктор с параметрами
    ///\param window[in]    - размер окна, должен быть больше значения shift, иначе используются значения по умолчанию
    ///\param shift[in]     - смещение окна, должно быть больше нуля и меньше размера окна, иначе используются значения по умолчанию
    ///\param K[in]         - коэфф. К для расчета сигмоиды
    ///\param r[in]         - коэфф. r для расчета сигмоиды
    ///\param Beta[in]      - коэфф. Beta для расчета сигмоиды
    ///\param queue[in]     - размер очереди гистограммы, должен быть больше 0
    Recognizer( const int window, const int shift, const float K, const float r, const float Beta, const int queue );
    /// деструктор
    ~Recognizer();
    /// передать в рекогнайзер записанные данные жестов для рассчета словаря классификатора, старый словарь удаляется
    ///\param gestsDB[in]   - записанные данные жестов
    void load( QVector<GestData> gestsDB);
    /// загрузить хэш матрицу
    /// передать в рекогнайзер записанные данные жестов для рассчета словаря классификатора с учетом хэширования, старый словарь удаляется
    ///\return true - матрица загружена, false - есть ошибка
    bool load( QString path, QVector<GestData> gestsDB );
    /// возвращает текст ошибки
    QString getError(){ return(m_error); }
public slots:
    /// слот приема входящего пакета
    ///\param packet[in]    - входящий декодированный myo пакет
    void _packet( MCU::SDU_MYOGRAPH_DATA packet );
    /// запустить в работу
    void _start();
    /// остановить работу
    void _stop();
signals:
    /// выдать распознанный жест
    ///\param gest[in] - номер жеста
    void s_gest(int gest);
};

class Controller: public QObject{
    Q_OBJECT
private:
    /// рабочий поток
    QThread     m_thread;
    /// признак работы
    bool        m_enable{false};
    /// текст ошибки
    QString     m_error{""};
public:
    /// конструктор
    Controller();
    /// деструктор
    ~Controller();
    /// запустить рекогнайзер из GUI в режиме NEIGHBOR
    ///\param window[in]    - размер окна, должен быть больше значения shift, иначе используются значения по умолчанию
    ///\param shift[in]     - смещение окна, должно быть больше нуля и меньше размера окна, иначе используются значения по умолчанию
    ///\param K[in]         - коэфф. К для расчета сигмоиды
    ///\param r[in]         - коэфф. r для расчета сигмоиды
    ///\param Beta[in]      - коэфф. Beta для расчета сигмоиды
    ///\param queue[in]     - размер очереди гистограммы, должен быть больше 0
    ///\param gestsDB[in]   - записанные данные жестов
    bool start(const int window, const int shift, const float K, const float r, const float Beta, const int queue, QVector<GestData> gestsDB);

    /// запустить рекогнайзер из GUI в режиме HASH
    ///\param window[in]    - размер окна, должен быть больше значения shift, иначе используются значения по умолчанию
    ///\param shift[in]     - смещение окна, должно быть больше нуля и меньше размера окна, иначе используются значения по умолчанию
    ///\param K[in]         - коэфф. К для расчета сигмоиды
    ///\param r[in]         - коэфф. r для расчета сигмоиды
    ///\param Beta[in]      - коэфф. Beta для расчета сигмоиды
    ///\param queue[in]     - размер очереди гистограммы, должен быть больше 0
    ///\param gestsDB[in]   - записанные данные жестов
    ///\param path          - полный путь к файлу матрицы
    ///\return true - успех, false - ошибка
    bool start(const int window, const int shift, const float K, const float r, const float Beta, const int queue, QVector<GestData> gestsDB, QString path);
    /// возвращает текст ошибки
    QString getError(){return(m_error);}
    /// отключить рекогнайзер из GUI
    void stop();
public slots:
    /// слот приема входящего пакета
    ///\param packet[in]    - входящий декодированный myo пакет
    void _packet( MCU::SDU_MYOGRAPH_DATA packet );
    /// слот приема распознанного жеста контроллером из воркера
    void _gest(int gest);
signals:
    /// сигнал входящего пакета для передачи в рекогнайзер
    ///\param packet[in]    - входящий декодированный myo пакет
    void s_packet( MCU::SDU_MYOGRAPH_DATA packet );
    /// сигнал распознанного жеста из контроллера в GUI
    ///\param gest[in] - номер жеста
    void s_gest(int gest);
};

/// ф-ция вычисления среднего по массиву
///\param data[in] - массив
///\return значение среднего
float average( QVector<float> & data );

/// ф-ция вычисления дисперсии по массиву и среднему
///\param data[in] - массив
///\param average[in] - среднее
///\return значение дисперсии
float dispersion( QVector<float> & data, float average );

/// ф-ция вычисления сигмоиды
///\param D[in] - значение дисперсии
///\param K[in] - коэфф
///\param r[in] - коэфф
///\param beta[in] - коэфф
///\return значение предобработки
float sigma( float D, float K, float r, float beta );

}


#endif // RECOGNIZER_H
