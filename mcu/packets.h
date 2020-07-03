/* utf8 */
#ifndef PACKETS_H
#define PACKETS_H
#include <QByteArray>
#include <QTextStream>
#include <QMetaType>
#include <QDebug>

/// количество датчиков на браслете
const int ELECTRODES_AMOUNT = 8;

namespace MCU{

//------------------------------------------------------------------------------
// константы
//------------------------------------------------------------------------------
// токены заголовка
const unsigned char token_B             = 'B';
const unsigned char token_L             = 'L';
// токены команд
const unsigned char UNDEFINED           = 0xff;
const unsigned char AUTONOMOUS          = 0x00;
const unsigned char MYOGRAPH            = 0x01;
const unsigned char LEARNING            = 0x02;
const unsigned char LEARNING_VECTOR     = 0x22;
const unsigned char LEARNING_PARAMS     = 0x42;
const unsigned char TESTING             = 0x03;
// токены ответов
const unsigned char AUTONOMOUS_ACK      = 0x10;
const unsigned char MYOGRAPH_ACK        = 0x11;
const unsigned char MYOGRAPH_DATA       = 'M';
const unsigned char LEARNING_ACK        = 0x12;
const unsigned char LEARNING_VECTOR_ACK = 0x32;
const unsigned char LEARNING_PARAMS_ACK = 0x52;
const unsigned char TESTING_ACK         = 0x13;
const unsigned char TESTING_DATA        = 0x23;

#pragma pack(push, 1)
/// SDU команды "AUTONOMOUS" (5 байт)
struct SDU_AUTONOMOUS_CMD {
    /// тэг команды "AUTONOMOUS"
    char        m_C{AUTONOMOUS};
    /// счетчик транзакций
    uint16_t    m_N{0};
    /// резерв, по умолчанию 0
    uint16_t    m_reserve{0};
    /// конструктор
    SDU_AUTONOMOUS_CMD();
    /// конструктор
    SDU_AUTONOMOUS_CMD(const SDU_AUTONOMOUS_CMD &other);
    /// деструктор
    ~SDU_AUTONOMOUS_CMD();
};
/// SDU команды "MYOGRAPH" (5 байт)
struct SDU_MYOGRAPH_CMD{
    /// тэг команды "MYOGRAPH"
    char        m_C{MYOGRAPH};
    /// счетчик транзакций
    uint16_t    m_N{0};
    /// резерв, по умолчанию 0
    uint16_t    m_reserve{0};
    /// конструктор
    SDU_MYOGRAPH_CMD();
    /// конструктор
    SDU_MYOGRAPH_CMD(const SDU_MYOGRAPH_CMD &other);
    /// деструктор
    ~SDU_MYOGRAPH_CMD();
};
/// SDU команды "LEARNING" (9 байт)
struct SDU_LEARNING_CMD {
    /// тэг команды "LEARNING"
    char        m_C{LEARNING};
    /// счетчик транзакций
    uint16_t    m_N{0};
    /// 1. Количество классов
    uint16_t    m_amount_classes{0};
    /// 2. Количество эталонных векторов на класс
    uint16_t    m_amount_vectors{0};
    /// 3. Резерв 0xYYYY
    uint16_t    m_reserve{0};
    /// конструктор
    SDU_LEARNING_CMD();
    /// конструктор
    SDU_LEARNING_CMD(const SDU_LEARNING_CMD &other);
    /// деструктор
    ~SDU_LEARNING_CMD();
};
/// SDU команды "LEARNING_VECTOR" ( 39 байт )
struct SDU_LEARNING_VECTOR_CMD {
    /// тэг команды "LEARNING_VECTOR"
    char        m_C{LEARNING_VECTOR};
    /// счетчик транзакций
    uint16_t    m_N{0};
    /// номер класса, которому принадлежит передаваемый вектор
    uint16_t    m_class_number{0};
    /// номер передаваемого вектора в классе
    uint16_t    m_vector_number{0};
    /// 8 каналов данных. Каналы в поле передаются по порядку, начиная с 1-го (float32_t или просто float 4 байта).
    /// канал №0
    float       m_channel0{0.0f};
    /// канал №1
    float       m_channel1{0.0f};
    /// канал №2
    float       m_channel2{0.0f};
    /// канал №3
    float       m_channel3{0.0f};
    /// канал №4
    float       m_channel4{0.0f};
    /// канал №5
    float       m_channel5{0.0f};
    /// канал №6
    float       m_channel6{0.0f};
    /// канал №7
    float       m_channel7{0.0f};
    /// конструктор
    SDU_LEARNING_VECTOR_CMD();
    /// конструктор
    SDU_LEARNING_VECTOR_CMD(const SDU_LEARNING_VECTOR_CMD &other);
    /// деструктор
    ~SDU_LEARNING_VECTOR_CMD();
};
/// SDU команды LEARNING_PARAMS
struct SDU_LEARNING_PARAMS_CMD {
    /// тэг команды "LEARNING_PARAMS"
    char        m_C{LEARNING_PARAMS};
    /// счетчик транзакций
    uint16_t    m_N{0};
    /// кэффициент K
    float       m_K{0.0f};
    /// кэффициент r
    float       m_r{0.0f};
    /// кэффициент Beta
    float       m_B{0.0f};
    /// ширина окна в количестве выборок
    uint16_t    m_Window{0};
    /// сдвиг окна в количестве выборок
    uint16_t    m_Shift{0};
    /// конструктор
    SDU_LEARNING_PARAMS_CMD();
    /// конструктор
    SDU_LEARNING_PARAMS_CMD(const SDU_LEARNING_PARAMS_CMD &other);
    /// деструктор
    ~SDU_LEARNING_PARAMS_CMD();
};

/// SDU команды "TESTING" (11 байт)
struct SDU_TESTING_CMD {
    /// тэг команды "TESTING"
    char        m_C{TESTING};
    /// счетчик транзакций
    uint16_t    m_N{0};
    /// 1 Ширина окна в количестве выборок
    uint16_t    m_window{0};
    /// 2 Сдвиг окна в количестве выборок
    uint16_t    m_offset{0};
    /// 3 Усреднение
    uint16_t    m_average{0};
    /// 4 Резерв
    uint16_t    m_reserve{0};
    /// конструктор
    SDU_TESTING_CMD();
    /// конструктор
    SDU_TESTING_CMD(const SDU_TESTING_CMD &other);
    /// деструктор
    ~SDU_TESTING_CMD();
};
#pragma pack(pop)

/// кодировать команду "AUTONOMOUS"
/// \param cmd[in]      - команда
/// \param sdu[in,out]  - кодированная команда
void encode_SDU_AUTONOMOUS_CMD( const SDU_AUTONOMOUS_CMD & cmd, QByteArray & sdu );
/// кодировать команду "MYOGRAPH"
/// \param cmd[in]      - команда
/// \param sdu[in,out]  - кодированная команда
void encode_SDU_MYOGRAPH_CMD( const SDU_MYOGRAPH_CMD & cmd, QByteArray & sdu );
/// кодировать команду "LEARNING"
/// \param cmd[in]      - команда
/// \param sdu[in,out]  - кодированная команда
void encode_SDU_LEARNING_CMD( const SDU_LEARNING_CMD & cmd, QByteArray & sdu );
/// кодировать команду "LEARNING_VECTOR"
/// \param cmd[in]      - команда
/// \param sdu[in,out]  - кодированная команда
void encode_SDU_LEARNING_VECTOR_CMD( const SDU_LEARNING_VECTOR_CMD & cmd, QByteArray & sdu );
/// кодировать команду "LEARNING_PARAMS"
/// \param cmd[in]      - команда
/// \param sdu[in,out]  - кодированная команда
void encode_SDU_LEARNING_PARAMS_CMD( const SDU_LEARNING_PARAMS_CMD & cmd, QByteArray & sdu );
/// кодировать команду "TESTING"
/// \param cmd[in]      - команда
/// \param sdu[in,out]  - кодированная команда
void encode_SDU_TESTING_CMD( const SDU_TESTING_CMD & cmd, QByteArray & sdu );

#pragma pack(push, 1)
/// SDU ACK команды "AUTONOMOUS" (5 байт)
struct SDU_AUTONOMOUS_ACK{
    /// тэг
    char        m_D{AUTONOMOUS_ACK};
    /// счетчик
    uint16_t    m_N{0};
    /// номер ошибки
    uint16_t    m_error{0};
    /// конструктор
    SDU_AUTONOMOUS_ACK();
    /// конструктор
    SDU_AUTONOMOUS_ACK(const SDU_AUTONOMOUS_ACK &other);
    /// деструктор
    ~SDU_AUTONOMOUS_ACK();
};
/// SDU ACK команды "MYOGRAPH" (5 байт)
struct SDU_MYOGRAPH_ACK{
    /// тэг
    char        m_D{MYOGRAPH_ACK};
    /// счетчик
    uint16_t    m_N{0};
    /// номер ошибки
    uint16_t    m_error{0};
    /// конструктор
    SDU_MYOGRAPH_ACK();
    /// конструктор
    SDU_MYOGRAPH_ACK(const SDU_MYOGRAPH_ACK &other);
    /// деструктор
    ~SDU_MYOGRAPH_ACK();
};
/// SDU DATA в режиме "MYOGRAPH" (37 байт)
struct SDU_MYOGRAPH_DATA{
    /// тэг
    char        m_D{MYOGRAPH_DATA};
    /// счетчик
    uint16_t    m_N{0};
    /// частота выборки Hz
    uint16_t    m_F{0};
    /// данные каналов АЦП
    float 		m_data[ ELECTRODES_AMOUNT ]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    /// конструктор
    SDU_MYOGRAPH_DATA();
    /// конструктор
    SDU_MYOGRAPH_DATA(const SDU_MYOGRAPH_DATA &other);
    /// деструктор
    ~SDU_MYOGRAPH_DATA();
};
/// SDU ACK команды "LEARNING" (11 байт)
struct SDU_LEARNING_ACK{
    /// тэг
    char        m_D{LEARNING_ACK};
    /// счетчик
    uint16_t    m_N{0};
    /// 1. Количество классов
    uint16_t    m_amount_classes{0};
    /// 2. Количество эталонных векторов на класс
    uint16_t    m_amount_vectors{0};
    /// 3. Номер ошибки
    uint16_t    m_error{0};
    /// 4. Резерв 0xYYYY
    uint16_t    m_reserve{0};
    /// конструктор
    SDU_LEARNING_ACK();
    /// конструктор
    SDU_LEARNING_ACK(const SDU_LEARNING_ACK &other);
    /// деструктор
    ~SDU_LEARNING_ACK();
};
/// SDU ACK команды "LEARNING_VECTOR" (5 байт)
struct SDU_LEARNING_VECTOR_ACK{
    /// тэг
    char        m_D{LEARNING_VECTOR_ACK};
    /// счетчик
    uint16_t    m_N{0};
    /// 1. Номер ошибки
    uint16_t    m_error{0};
    /// конструктор
    SDU_LEARNING_VECTOR_ACK();
    /// конструктор
    SDU_LEARNING_VECTOR_ACK(const SDU_LEARNING_VECTOR_ACK &other);
    /// деструктор
    ~SDU_LEARNING_VECTOR_ACK();
};
/// SDU ACK команды "LEARNING_PARAMS" ( байт)
struct SDU_LEARNING_PARAMS_ACK{
    /// тэг
    char        m_D{LEARNING_PARAMS_ACK};
    /// счетчик
    uint16_t    m_N{0};
    /// 1. Номер ошибки
    uint16_t    m_error{0};
    /// конструктор
    SDU_LEARNING_PARAMS_ACK();
    /// конструктор
    SDU_LEARNING_PARAMS_ACK(const SDU_LEARNING_PARAMS_ACK &other);
    /// деструктор
    ~SDU_LEARNING_PARAMS_ACK();
};
/// SDU ACK команды "TESTING" (5 байт)
struct SDU_TESTING_ACK{
    /// тэг
    char        m_D{TESTING_ACK};
    /// счетчик
    uint16_t    m_N{0};
    /// № ошибки
    uint16_t    m_error{0};
    /// конструктор
    SDU_TESTING_ACK();
    /// конструктор
    SDU_TESTING_ACK(const SDU_TESTING_ACK &other);
    /// деструктор
    ~SDU_TESTING_ACK();
};
/// SDU DATA команды "TESTING" (13 байт)
struct SDU_TESTING_DATA{
    /// тэг
    char        m_D{TESTING_DATA};
    /// счетчик
    uint16_t    m_N{0};
    /// Номер распознанного хвата
    uint16_t    m_gest{0};
    /// Ширина окна в количестве выборок
    uint16_t    m_window{0};
    /// Сдвиг окна в количестве выборок
    uint16_t    m_offset{0};
    /// Усреднение
    uint16_t    m_average{0};
    /// Резерв
    uint16_t    m_reserve{0};
    /// конструктор
    SDU_TESTING_DATA();
    /// конструктор
    SDU_TESTING_DATA(const SDU_TESTING_DATA &other);
    /// деструктор
    ~SDU_TESTING_DATA();
};
#pragma pack(pop)

/// декодировать ACK команды "AUTONOMOUS"
/// \param sdu[in]      - кодированный SDU
/// \param ack[in,out]  - декодированный ACK
bool decode_SDU_AUTONOMOUS_ACK( const QByteArray & sdu, SDU_AUTONOMOUS_ACK & ack );
/// декодировать ACK команды "MYOGRAPH"
/// \param sdu[in]      - кодированный SDU
/// \param ack[in,out]  - декодированный ACK
bool decode_SDU_MYOGRAPH_ACK( const QByteArray & sdu, SDU_MYOGRAPH_ACK & ack );
/// декодировать данные команды "MYOGRAPH"
/// \param sdu[in]      - кодированный SDU
/// \param data[in,out] - декодированные данные
bool decode_SDU_MYOGRAPH_DATA( const QByteArray & sdu, SDU_MYOGRAPH_DATA & data );
/// декодировать ACK команды "LEARNING"
/// \param sdu[in]      - кодированный SDU
/// \param ack[in,out]  - декодированный ACK
bool decode_SDU_LEARNING_ACK( const QByteArray & sdu, SDU_LEARNING_ACK & ack );
/// декодировать ACK команды "LEARNING_VECTOR"
/// \param sdu[in]      - кодированный SDU
/// \param ack[in,out]  - декодированный ACK
bool decode_SDU_LEARNING_VECTOR_ACK(const QByteArray & sdu, SDU_LEARNING_VECTOR_ACK & ack );
/// декодировать ACK команды "LEARNING_PARAMS"
/// \param sdu[in]      - кодированный SDU
/// \param ack[in,out]  - декодированный ACK
bool decode_SDU_LEARNING_PARAMS_ACK(const QByteArray & sdu, SDU_LEARNING_PARAMS_ACK & ack );
/// декодировать ACK команды "TESTING"
/// \param sdu[in]      - кодированный SDU
/// \param ack[in,out]  - декодированный ACK
bool decode_SDU_TESTING_ACK(const QByteArray & sdu, SDU_TESTING_ACK &ack );
/// декодировать данные команды "TESTING"
/// \param sdu[in]      - кодированный SDU
/// \param data[in,out] - декодированные данные
bool decode_SDU_TESTING_DATA(const QByteArray & sdu, SDU_TESTING_DATA &data );


/// Вычисляет CRC по полиному 0xA001
///\param crc[in]   - предыдущее значение CRC
///\param a[in]     - входной байт
///\return обновленное значение CRC
uint16_t crc16_update_poly( uint16_t crc , uint8_t a );
/// Вычисляет CRC блока
///\param crc[in]   - предыдущее значение CRC
///\param a[in]     -  адрес первого байта последовательности
///\param len[in]   - длина последовательности
///\return обновленное значение CRC
uint16_t crc16_update_blk( uint16_t crc , uint8_t* a , uint32_t len );
/// кодирование PDU
/// \param sdu[in]  - входящий SDU
/// \return байтпоследовательность PDU
QByteArray encodePDU( const QByteArray & sdu );


QDebug& operator<<(QDebug& dbg, const SDU_AUTONOMOUS_CMD &cmd );
QDebug& operator<<(QDebug& dbg, const SDU_MYOGRAPH_CMD &cmd );
QDebug& operator<<(QDebug& dbg, const SDU_LEARNING_CMD &cmd );
QDebug& operator<<(QDebug& dbg, const SDU_LEARNING_VECTOR_CMD &cmd );
QDebug& operator<<(QDebug& dbg, const SDU_LEARNING_PARAMS_CMD &cmd );
QDebug& operator<<(QDebug& dbg, const SDU_TESTING_CMD &cmd );

QDebug& operator<<(QDebug& dbg, const SDU_AUTONOMOUS_ACK &ack );
QDebug& operator<<(QDebug& dbg, const SDU_MYOGRAPH_ACK &ack );
QDebug& operator<<(QDebug& dbg, const SDU_MYOGRAPH_DATA &data);
QDebug& operator<<(QDebug& dbg, const SDU_LEARNING_ACK &ack );
QDebug& operator<<(QDebug& dbg, const SDU_LEARNING_VECTOR_ACK &data );
QDebug& operator<<(QDebug& dbg, const SDU_LEARNING_PARAMS_ACK &data );
QDebug& operator<<(QDebug& dbg, const SDU_TESTING_ACK &ack );
QDebug& operator<<(QDebug& dbg, const SDU_TESTING_DATA &data );

QTextStream& operator<<(QTextStream& stream, const SDU_TESTING_DATA &data );
QTextStream& operator<<(QTextStream& stream, const SDU_MYOGRAPH_DATA &data );
QTextStream& operator<<(QTextStream& stream, const SDU_LEARNING_ACK &data );
QTextStream& operator<<(QTextStream& stream, const SDU_LEARNING_VECTOR_ACK &data );
QTextStream& operator<<(QTextStream& stream, const SDU_LEARNING_PARAMS_ACK &data );
}

Q_DECLARE_METATYPE(MCU::SDU_AUTONOMOUS_CMD)
Q_DECLARE_METATYPE(MCU::SDU_MYOGRAPH_CMD)
Q_DECLARE_METATYPE(MCU::SDU_LEARNING_CMD)
Q_DECLARE_METATYPE(MCU::SDU_LEARNING_VECTOR_CMD)
Q_DECLARE_METATYPE(MCU::SDU_LEARNING_PARAMS_CMD)
Q_DECLARE_METATYPE(MCU::SDU_TESTING_CMD)

Q_DECLARE_METATYPE(MCU::SDU_AUTONOMOUS_ACK)
Q_DECLARE_METATYPE(MCU::SDU_MYOGRAPH_ACK)
Q_DECLARE_METATYPE(MCU::SDU_MYOGRAPH_DATA)
Q_DECLARE_METATYPE(MCU::SDU_LEARNING_ACK)
Q_DECLARE_METATYPE(MCU::SDU_LEARNING_VECTOR_ACK)
Q_DECLARE_METATYPE(MCU::SDU_LEARNING_PARAMS_ACK)
Q_DECLARE_METATYPE(MCU::SDU_TESTING_ACK)
Q_DECLARE_METATYPE(MCU::SDU_TESTING_DATA)

#endif // PACKETS_H
