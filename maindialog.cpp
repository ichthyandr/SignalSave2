/*utf8*/
#include "maindialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QStatusBar>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>
#include <QMessageBox>
#include <QPainter>
#include <QLineEdit>
#include <QFileDialog>

// for logging
Q_LOGGING_CATEGORY(GUI, "GUI")

// object names
static const QString _statusBar     = QString::fromUtf8("statusBar");
static const QString _cbxHand       = QString::fromUtf8("comboboxHand");
static const QString _imageLabel    = QString::fromUtf8("imageLabel");
static const QString _pbtnStart     = QString::fromUtf8("pbtnStart");
static const QString _pbtnRecord    = QString::fromUtf8("pbtnRecord");
static const QString _pbtnReset     = QString::fromUtf8("pbtnReset");
static const QString _pbtnTest      = QString::fromUtf8("pbtnTest");
static const QString _pbtnStopTst   = QString::fromUtf8("pbtnStopTst");
static const QString _pbtnSave      = QString::fromUtf8("pbtnSave");
static const QString _peditMatrix   = QString::fromUtf8("peditMatrix");

static const QString _myoPortWidget = QString::fromUtf8("myoPortWidget");

////////////////////////////////////////////////////////////////////////////
/// реализация MainDialog

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent){
    qCDebug(GUI) << Q_FUNC_INFO;

    //////////////////////////////////////////////////////////////////////////////////////
    /// основной лэйаут
    auto layout = new QVBoxLayout( this );

    //////////////////////////////////////////////////////////////////////////////////////
    /// верх
    auto hbx1 = new QHBoxLayout();
    layout->addLayout(hbx1);
    layout->setAlignment(hbx1,Qt::AlignTop);

    auto lblGain        = new QLabel(QString::fromUtf8("Усиление"),this);
    auto spbxGain       = new QSpinBox(this);
    spbxGain->setRange(1,300);
    spbxGain->setValue( m_params.m_gain );

    auto lblVectors     = new QLabel(QString::fromUtf8("Повторений"),this);
    auto spbxVectors    = new QSpinBox(this);
    spbxVectors->setRange(1,20);
    spbxVectors->setValue( m_params.m_vectors );

    auto lblWindow      = new QLabel(QString::fromUtf8("Окно"),this);
    auto spbxWindow     = new QSpinBox(this);
    spbxWindow->setRange(10,1000);
    spbxWindow->setValue( m_params.m_window );

    auto lblShift       = new QLabel(QString::fromUtf8("Сдвиг"),this);
    auto spbxShift      = new QSpinBox(this);
    spbxShift->setRange(1,1000);
    spbxShift->setValue( m_params.m_shift );

    auto lblK           = new QLabel(QString::fromUtf8("K"),this);
    auto spbxK          = new QSpinBox(this);
    //spbxK->setDecimals( 2 );
    //spbxK->setRange( 0.01, 100.00 );
    //spbxK->setSingleStep( 0.01 );
    spbxK->setRange(1,1000000);
    spbxK->setValue( m_params.m_K );

    auto lblR           = new QLabel(QString::fromUtf8("r"),this);
    auto spbxR          = new QSpinBox(this);
    //spbxR->setDecimals( 2 );
    //spbxR->setRange( 0.01, 100.00 );
    //spbxR->setSingleStep( 0.01 );
    spbxR->setRange(1,100);
    spbxR->setValue( m_params.m_r );

    auto lblBeta        = new QLabel(QString::fromUtf8("Beta"),this);
    auto spbxBeta       = new QDoubleSpinBox(this);
    spbxBeta->setDecimals( 2 );
    spbxBeta->setRange( 0.01, 100.00 );
    spbxBeta->setSingleStep( 0.01 );
    spbxBeta->setValue( m_params.m_Beta );

    auto lblHistogram   = new QLabel(QString::fromUtf8("Гистограмма"),this);
    auto spbxHistogram  = new QSpinBox(this);
    spbxHistogram->setRange(1,100);
    spbxHistogram->setValue( m_params.m_histogram_queue );

    auto peditMatrix = new QLineEdit( this );
    //peditMatrix->setDisabled( true );
    peditMatrix->setObjectName( _peditMatrix );
    peditMatrix->setText(m_params.m_matrix);
    auto pbtnMatrix = new QPushButton(QString("Матрица"));

    auto lblHand        = new QLabel(QString::fromUtf8("Рука"),this);
    auto cbxHand        = new QComboBox(this);
    cbxHand->setObjectName( _cbxHand );

    cbxHand->addItem("Левая", QVariant(true));
    cbxHand->addItem("Правая", QVariant(false));
    cbxHand->setCurrentIndex( 0 );

    hbx1->addWidget(lblGain);
    hbx1->addWidget(spbxGain);
    hbx1->addSpacing(7);

    hbx1->addWidget(lblVectors);
    hbx1->addWidget(spbxVectors);
    hbx1->addSpacing(7);

    hbx1->addWidget(lblWindow);
    hbx1->addWidget(spbxWindow);
    hbx1->addSpacing(7);

    hbx1->addWidget(lblShift);
    hbx1->addWidget(spbxShift);
    hbx1->addSpacing(7);

    hbx1->addWidget(lblK);
    hbx1->addWidget(spbxK);
    hbx1->addSpacing(7);

    hbx1->addWidget(lblR);
    hbx1->addWidget(spbxR);
    hbx1->addSpacing(7);

    hbx1->addWidget(lblBeta);
    hbx1->addWidget(spbxBeta);
    hbx1->addSpacing(7);

    hbx1->addWidget(lblHistogram);
    hbx1->addWidget(spbxHistogram);
    hbx1->addSpacing(7);

    hbx1->addWidget(peditMatrix);
    hbx1->addWidget(pbtnMatrix);

    hbx1->addStretch(10000);

    hbx1->addWidget(lblHand);
    hbx1->addWidget(cbxHand);

    //m_paramsGroup << spbxGain << spbxVectors << spbxWindow << spbxShift << spbxK << spbxR << spbxBeta << spbxHistogram << cbxHand << pbtnMatrix;
    m_paramsGroup << spbxGain << spbxVectors << spbxWindow << spbxK << spbxR << spbxBeta << cbxHand << pbtnMatrix;
    /// верх
    //////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////////
    /// середина
    auto hbx2 = new QHBoxLayout();
    layout->addLayout(hbx2);

    /// жесты
    auto vbx1 = new QVBoxLayout();
    hbx2->addLayout(vbx1);
    hbx2->setAlignment(vbx1,Qt::AlignLeft);


    /// группа жестов
    auto gbGests = new QGroupBox( this );
    gbGests->setTitle(QString::fromUtf8("Жесты"));

    QSizePolicy policy;
    policy.setVerticalPolicy(QSizePolicy::Expanding);
    gbGests->setSizePolicy(policy);

    vbx1->addWidget( gbGests,0, Qt::AlignTop );

    auto vblGests = new QVBoxLayout();
    gbGests->setLayout( vblGests );

    auto gestChBx1  = new QCheckBox(QString::fromUtf8("Расслабление"), this);
    auto gestChBx2  = new QCheckBox(QString::fromUtf8("Щепоть"), this);
    auto gestChBx3  = new QCheckBox(QString::fromUtf8("Кулак"), this);
    auto gestChBx4  = new QCheckBox(QString::fromUtf8("Цилиндр"), this);
    auto gestChBx5  = new QCheckBox(QString::fromUtf8("Движение пальца вниз"), this);
    auto gestChBx6  = new QCheckBox(QString::fromUtf8("Движение пальца вверх"), this);
    auto gestChBx7  = new QCheckBox(QString::fromUtf8("Раскрытие"), this);
    auto gestChBx8  = new QCheckBox(QString::fromUtf8("Класс"), this);
    auto gestChBx9  = new QCheckBox(QString::fromUtf8("Приветствие"), this);
    auto gestChBx10 = new QCheckBox(QString::fromUtf8("Победа"), this);
    auto gestChBx11 = new QCheckBox(QString::fromUtf8("Поворот от себя"), this);
    auto gestChBx12 = new QCheckBox(QString::fromUtf8("Поворот на себя"), this);
    auto gestChBx13 = new QCheckBox(QString::fromUtf8("Вращение по часовой стрелке"), this);
    auto gestChBx14 = new QCheckBox(QString::fromUtf8("Вращение против часовой стрелки"), this);

    vblGests->addWidget( gestChBx1 );
    vblGests->addWidget( gestChBx2 );
    vblGests->addWidget( gestChBx3 );
    vblGests->addWidget( gestChBx4 );
    vblGests->addWidget( gestChBx5 );
    vblGests->addWidget( gestChBx6 );
    vblGests->addWidget( gestChBx7 );
    vblGests->addWidget( gestChBx8 );
    vblGests->addWidget( gestChBx9 );
    vblGests->addWidget( gestChBx10 );
    vblGests->addWidget( gestChBx11 );
    vblGests->addWidget( gestChBx12 );
    vblGests->addWidget( gestChBx13 );
    vblGests->addWidget( gestChBx14 );

    connect( gestChBx1,     &QCheckBox::toggled, this,  &MainDialog::_gestChBx1);
    connect( gestChBx2,     &QCheckBox::toggled, this,  &MainDialog::_gestChBx2);
    connect( gestChBx3,     &QCheckBox::toggled, this,  &MainDialog::_gestChBx3);
    connect( gestChBx4,     &QCheckBox::toggled, this,  &MainDialog::_gestChBx4);
    connect( gestChBx5,     &QCheckBox::toggled, this,  &MainDialog::_gestChBx5);
    connect( gestChBx6,     &QCheckBox::toggled, this,  &MainDialog::_gestChBx6);
    connect( gestChBx7,     &QCheckBox::toggled, this,  &MainDialog::_gestChBx7);
    connect( gestChBx8,     &QCheckBox::toggled, this,  &MainDialog::_gestChBx8);
    connect( gestChBx9,     &QCheckBox::toggled, this,  &MainDialog::_gestChBx9);
    connect( gestChBx10,    &QCheckBox::toggled, this,  &MainDialog::_gestChBx10);
    connect( gestChBx11,    &QCheckBox::toggled, this,  &MainDialog::_gestChBx11);
    connect( gestChBx12,    &QCheckBox::toggled, this,  &MainDialog::_gestChBx12);
    connect( gestChBx13,    &QCheckBox::toggled, this,  &MainDialog::_gestChBx13);
    connect( gestChBx14,    &QCheckBox::toggled, this,  &MainDialog::_gestChBx14);

    m_paramsGroup << gestChBx1 << gestChBx2 << gestChBx3 << gestChBx4 << gestChBx5 << gestChBx6 << gestChBx7
                  << gestChBx8 << gestChBx9 << gestChBx10 << gestChBx11 << gestChBx12 << gestChBx13 << gestChBx14;

    /// группа управления браслетом
    auto gbMyo = new QGroupBox( this );
    gbMyo->setTitle(QString::fromUtf8("Браслет"));

    QSizePolicy policyMyo;
     policyMyo.setHorizontalPolicy(QSizePolicy::Expanding);
    gbMyo->setSizePolicy(policyMyo);
    vbx1->addWidget( gbMyo,0, Qt::AlignTop );

    auto vblMyo = new QVBoxLayout();
    gbMyo->setLayout( vblMyo );

    auto myoPort = new portWidget(_myoPortWidget, this, true );
    vblMyo->addWidget( myoPort );

    // для MYO
    connect( myoPort,   &portWidget::s_onMyo,                   this,       &MainDialog::_connectMYO );
    connect( myoPort,   &portWidget::s_offMyo,                  this,       &MainDialog::_disconnectMYO );
    connect( &m_myo,    &MYO::Controller::s_myograph_data,      this,       &MainDialog::_packet );
    connect( &m_myo,    &MYO::Controller::s_myograph_data,      &m_rcgn,    &MATH::Controller::_packet );

    // для MCU
    connect( myoPort,   &portWidget::s_on,                      this,       &MainDialog::_connectMCU );
    connect( myoPort,   &portWidget::s_off,                     this,       &MainDialog::_disconnectMCU );
    connect( &m_mcu,    &MCUBION::Controller::s_myograph_data,  this,       &MainDialog::_packet );
    connect( &m_mcu,    &MCUBION::Controller::s_myograph_data,  &m_rcgn,    &MATH::Controller::_packet );
    connect( &m_mcu,    &MCUBION::Controller::s_state,          this,       &MainDialog::_state );

    vbx1->addStretch(1000);


    /// картинка
    auto vbx2 = new QVBoxLayout();
    hbx2->addLayout(vbx2);
    hbx2->setAlignment(vbx2,Qt::AlignLeft);

    auto hbx21 = new QHBoxLayout();
    vbx2->addLayout(hbx21);
    vbx2->setAlignment(hbx21,Qt::AlignTop);

    auto imageLabel = new QLabel();
    imageLabel->setObjectName(_imageLabel);
    hbx21->addWidget(imageLabel);
    hbx21->addStretch(1000);

    /// середина
    //////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////////
    /// низ
    auto hbx3 = new QHBoxLayout();
    layout->addLayout(hbx3);
    layout->setAlignment(hbx3,Qt::AlignBottom);

    auto pbtnStart  = new QPushButton(QString::fromUtf8("Пуск"),this);          pbtnStart->setObjectName(_pbtnStart);
    auto pbtnRecord = new QPushButton(QString::fromUtf8("Запись"),this);        pbtnRecord->setObjectName(_pbtnRecord);
    auto pbtnReset  = new QPushButton(QString::fromUtf8("Сброс"),this);         pbtnReset->setObjectName(_pbtnReset);
    auto pbtnTest   = new QPushButton(QString::fromUtf8("Тест"),this);          pbtnTest->setObjectName(_pbtnTest);
    auto pbtnStopTst= new QPushButton(QString::fromUtf8("Стоп"),this);          pbtnStopTst->setObjectName(_pbtnStopTst);
    auto pbtnSave   = new QPushButton(QString::fromUtf8("Сохранить"),this);     pbtnSave->setObjectName(_pbtnSave);

    hbx3->addStretch(10000);
    hbx3->addWidget( pbtnStart );
    hbx3->addSpacing(20);
    hbx3->addWidget( pbtnRecord );
    hbx3->addSpacing(20);
    hbx3->addWidget( pbtnReset );
    hbx3->addSpacing(20);
    hbx3->addWidget( pbtnTest );
    hbx3->addSpacing(20);
    hbx3->addWidget( pbtnStopTst );
    hbx3->addSpacing(20);
    hbx3->addWidget( pbtnSave );

    /// низ
    //////////////////////////////////////////////////////////////////////////////////////

    connect( spbxGain,      static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),               this, &MainDialog::_gain );
    connect( spbxVectors,   static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),               this, &MainDialog::_vectors );
    connect( spbxWindow,    static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),               this, &MainDialog::_window );
    connect( spbxShift,     static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),               this, &MainDialog::_shift );
    connect( spbxK,         static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),               this, &MainDialog::_K );
    connect( spbxR,         static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),               this, &MainDialog::_r );
    connect( spbxBeta,      static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),this, &MainDialog::_Beta );
    connect( spbxHistogram, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),               this, &MainDialog::_histogram );
    connect( peditMatrix,   &QLineEdit::textChanged,                                                    this, &MainDialog::_matrix );
    connect( pbtnMatrix,    &QPushButton::clicked,                                                      this, &MainDialog::_file);
    connect( cbxHand,       static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),      this, &MainDialog::_hand );

    connect( pbtnStart,     &QPushButton::clicked, this,	&MainDialog::_start );
    connect( pbtnRecord,    &QPushButton::clicked, this,	&MainDialog::_record );
    connect( pbtnReset,     &QPushButton::clicked, this,	&MainDialog::_reset );
    connect( pbtnTest,      &QPushButton::clicked, this,	&MainDialog::_test );
    connect( pbtnStopTst,   &QPushButton::clicked, this,	&MainDialog::_stopTst );
    connect( pbtnSave,      &QPushButton::clicked, this,	&MainDialog::_save );

    connect( this,          &MainDialog::s_startRecord, this, &MainDialog::_startRecord );
    connect( this,          &MainDialog::s_stopRecord,  this, &MainDialog::_stopRecord );

    //////////////////////////////////////////////////////////////////////////////////////
    /// прогресс индикатор
    auto hbx4 = new QHBoxLayout();
    layout->addLayout(hbx4);
    layout->setAlignment(hbx4,Qt::AlignBottom);

    m_pProgress = new QProgressBar(this);
    m_pProgress->setTextVisible(false);
    hbx4->addWidget( m_pProgress );

    //////////////////////////////////////////////////////////////////////////////////////
    /// индикаторы датчиков
    auto hbx5 = new QHBoxLayout();
    layout->addLayout(hbx5);
    layout->setAlignment(hbx5,Qt::AlignBottom);

    auto plbl0 = new QLabel("0", this); plbl0->setAlignment(Qt::AlignRight); //plbl0->setFrameStyle(QFrame::Plain);
    auto plbl1 = new QLabel("0", this); plbl1->setAlignment(Qt::AlignRight); //plbl1->setFrameStyle(QFrame::Plain);
    auto plbl2 = new QLabel("0", this); plbl2->setAlignment(Qt::AlignRight); //plbl2->setFrameStyle(QFrame::Plain);
    auto plbl3 = new QLabel("0", this); plbl3->setAlignment(Qt::AlignRight); //plbl3->setFrameStyle(QFrame::Plain);
    auto plbl4 = new QLabel("0", this); plbl4->setAlignment(Qt::AlignRight); //plbl4->setFrameStyle(QFrame::Plain);
    auto plbl5 = new QLabel("0", this); plbl5->setAlignment(Qt::AlignRight); //plbl5->setFrameStyle(QFrame::Plain);
    auto plbl6 = new QLabel("0", this); plbl6->setAlignment(Qt::AlignRight); //plbl6->setFrameStyle(QFrame::Plain);
    auto plbl7 = new QLabel("0", this); plbl7->setAlignment(Qt::AlignRight); //plbl7->setFrameStyle(QFrame::Plain);

    hbx5->addWidget( plbl0 );
    hbx5->addWidget( plbl1 );
    hbx5->addWidget( plbl2 );
    hbx5->addWidget( plbl3 );
    hbx5->addWidget( plbl4 );
    hbx5->addWidget( plbl5 );
    hbx5->addWidget( plbl6 );
    hbx5->addWidget( plbl7 );

    m_sensors << plbl0 << plbl1 << plbl2 << plbl3 << plbl4 << plbl5 << plbl6 << plbl7;
    //////////////////////////////////////////////////////////////////////////////////////
    /// подвал - добавить строку состояния
    auto statusBar = new QStatusBar( this );
    layout ->addWidget( statusBar );
    statusBar->setObjectName(_statusBar);

    printStatus(QString::fromUtf8("Готов"));

    //////////////////////////////////////////////////////////////////////////////////////
    /// инициализация формы и таймер

    update();

    m_pTimer = new QTimer( this );
    m_pTimer->setInterval( TIMEOUT );
    m_pTimer->setSingleShot( true );

    connect( m_pTimer, &QTimer::timeout,            this, &MainDialog::_timeout );
    connect( &m_rcgn,  &MATH::Controller::s_gest,   this, &MainDialog::_gest );

    //////////////////////////////////////////////////////////////////////////////////////
    /// загрузить картинки
    for ( int gest=1; gest <= GESTURES_AMOUNT; gest++ ){
        QString fileName = QString(":/%1gest").arg( gest,3,10,QLatin1Char('0'));
        QImage image(fileName);
        QImage image1 = image.scaled( 987,692, Qt::IgnoreAspectRatio);

        m_listImages << image1;
    }
}

MainDialog::~MainDialog(){
    qCDebug(GUI) << Q_FUNC_INFO;
    m_myo.off();
    m_mcu.off();
}

void MainDialog::printStatus( const QString & status ){
    qCDebug(GUI) << Q_FUNC_INFO << status;
    auto statusBar  = findChild<QStatusBar*>(_statusBar);
    if ( statusBar )
        statusBar->showMessage( status );
    else
        qCCritical(GUI) << Q_FUNC_INFO << "can't find statusBar!";
}

void MainDialog::_matrix(const QString &text){
    qCDebug(GUI) << Q_FUNC_INFO << "text=" << text;
    m_params.m_matrix = text;
}

void MainDialog::_file(){

    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", tr("Data (*.matrix)"));
    qCDebug(GUI) << Q_FUNC_INFO << fileName;

    auto peditMatrix  = findChild<QLineEdit*>(_peditMatrix);
    if ( 0 != peditMatrix ){
        peditMatrix->setText( fileName );
        m_params.m_matrix = fileName;
    }
    else
        qCCritical(GUI) << Q_FUNC_INFO << "can't find peditMatrix!";
}

void MainDialog::_gestChBx1(bool checked){
    qCDebug(GUI) << Q_FUNC_INFO << "checked=" << checked;
    m_params.m_gests[0] = checked;
}

void MainDialog::_gestChBx2(bool checked){
    qCDebug(GUI) << Q_FUNC_INFO << "checked=" << checked;
    m_params.m_gests[1] = checked;
}

void MainDialog::_gestChBx3(bool checked){
    qCDebug(GUI) << Q_FUNC_INFO << "checked=" << checked;
    m_params.m_gests[2] = checked;
}

void MainDialog::_gestChBx4(bool checked){
    qCDebug(GUI) << Q_FUNC_INFO << "checked=" << checked;
    m_params.m_gests[3] = checked;
}

void MainDialog::_gestChBx5(bool checked){
    qCDebug(GUI) << Q_FUNC_INFO << "checked=" << checked;
    m_params.m_gests[4] = checked;
}

void MainDialog::_gestChBx6(bool checked){
    qCDebug(GUI) << Q_FUNC_INFO << "checked=" << checked;
    m_params.m_gests[5] = checked;
}

void MainDialog::_gestChBx7(bool checked){
    qCDebug(GUI) << Q_FUNC_INFO << "checked=" << checked;
    m_params.m_gests[6] = checked;
}

void MainDialog::_gestChBx8(bool checked){
    qCDebug(GUI) << Q_FUNC_INFO << "checked=" << checked;
    m_params.m_gests[7] = checked;
}

void MainDialog::_gestChBx9(bool checked){
    qCDebug(GUI) << Q_FUNC_INFO << "checked=" << checked;
    m_params.m_gests[8] = checked;
}

void MainDialog::_gestChBx10(bool checked){
    qCDebug(GUI) << Q_FUNC_INFO << "checked=" << checked;
    m_params.m_gests[9] = checked;
}

void MainDialog::_gestChBx11(bool checked){
    qCDebug(GUI) << Q_FUNC_INFO << "checked=" << checked;
    m_params.m_gests[10] = checked;
}

void MainDialog::_gestChBx12(bool checked){
    qCDebug(GUI) << Q_FUNC_INFO << "checked=" << checked;
    m_params.m_gests[11] = checked;
}

void MainDialog::_gestChBx13(bool checked){
    qCDebug(GUI) << Q_FUNC_INFO << "checked=" << checked;
    m_params.m_gests[12] = checked;
}

void MainDialog::_gestChBx14(bool checked){
    qCDebug(GUI) << Q_FUNC_INFO << "checked=" << checked;
    m_params.m_gests[13] = checked;
}

void MainDialog::_gain( int value ){
    qCDebug(GUI) << Q_FUNC_INFO << "value=" << value;
    m_params.m_gain = value;
}

void MainDialog::_vectors( int value ){
    qCDebug(GUI) << Q_FUNC_INFO << "value=" << value;
    m_params.m_vectors = value;
}

void MainDialog::_window( int value ){
    qCDebug(GUI) << Q_FUNC_INFO << "value=" << value;
    m_params.m_window = value;
}

void MainDialog::_shift( int value ){
    qCDebug(GUI) << Q_FUNC_INFO << "value=" << value;
    m_params.m_shift = value;
}

void MainDialog::_K( int value ){
    qCDebug(GUI) << Q_FUNC_INFO << "value=" << value;
    m_params.m_K = value;
}

void MainDialog::_r( int value ){
    qCDebug(GUI) << Q_FUNC_INFO << "value=" << value;
    m_params.m_r = value;
}

void MainDialog::_Beta( double value ){
    qCDebug(GUI) << Q_FUNC_INFO << "value=" << value;
    m_params.m_Beta = value;
}

void MainDialog::_histogram( int value ){
    qCDebug(GUI) << Q_FUNC_INFO << "value=" << value;
    m_params.m_histogram_queue = value;
}

void MainDialog::_hand( int index ){
    qCDebug(GUI) << Q_FUNC_INFO << "index=" << index;
    auto cbxHand = findChild<QComboBox*>( _cbxHand );
    if ( cbxHand ){
        m_params.m_leftHand = cbxHand->itemData(index).toBool();
        qCDebug(GUI) << Q_FUNC_INFO << "leftHand=" << m_params.m_leftHand;
    }
    else
        qCCritical(GUI) << Q_FUNC_INFO << "can't find combobox hand!";
}

void MainDialog::_start( ){
    qCDebug(GUI) << Q_FUNC_INFO;
    m_gests.clear();
    int gest = 1;
    for( bool learn: m_params.m_gests ){
        if ( learn ) m_gests << gest;
        gest++;
    }
    if ( 0 ==  m_gests.size() ){
        QMessageBox::critical(this, "SignalSave2", QString::fromUtf8("Жесты для обучения не выбраны!"));
        qCCritical(GUI) << Q_FUNC_INFO << "no gests to learn!";
        return;
    }
    m_state = LRN_READY;
    update();
}

void MainDialog::_record( ){
    qCDebug(GUI) << Q_FUNC_INFO;
    m_state = LRN_RECORDING;
    update();
    emit s_startRecord();
}

void MainDialog::_reset( ){
    qCDebug(GUI) << Q_FUNC_INFO;
    m_state = LRN_NODATA;
    update();
    m_gestsDB.clear();
}

void MainDialog::_test( ){
    qCDebug(GUI) << Q_FUNC_INFO;
    bool res;
    if ( m_params.m_matrix != "" ){
        res = m_rcgn.start(m_params.m_window, m_params.m_shift, m_params.m_K, m_params.m_r, m_params.m_Beta, m_params.m_histogram_queue, m_gestsDB, m_params.m_matrix);
    }
    else {
        res = m_rcgn.start(m_params.m_window, m_params.m_shift, m_params.m_K, m_params.m_r, m_params.m_Beta, m_params.m_histogram_queue, m_gestsDB);
    }
    if ( res ){
        m_state = LRN_TESTING;
        update();
    }
    else {
        QString error = m_rcgn.getError();
        QMessageBox::critical(this, "SignalSave2", error);
    }
}

void MainDialog::_stopTst(){
    qCDebug(GUI) << Q_FUNC_INFO;
    m_rcgn.stop();
    m_state = LRN_DONE;
    update();
}

void MainDialog::_startRecord(){
    qCDebug(GUI) << Q_FUNC_INFO;

    GestData gestData;
    m_gestData = gestData;
    m_gestData.m_gest   = m_gests.at(0);
    m_gestData.m_number = m_repeat;

    if ( Q_NULLPTR != m_pTimer ){
        m_pTimer->start();
    }
    else
        qCCritical(GUI) << Q_FUNC_INFO << "timer not found!";

    if ( Q_NULLPTR != m_pProgress ){
        m_pProgress->setMinimum( 0 );
        m_pProgress->setMaximum( m_params.m_window );
        m_pProgress->setValue(0);
    }
    else
        qCCritical(GUI) << Q_FUNC_INFO << "progressbar not found!";
    //QTimer::singleShot(1000, [=]() { emit s_stopRecord() ; } );
}

void MainDialog::_stopRecord(){
    qCDebug(GUI) << Q_FUNC_INFO;

    if ( Q_NULLPTR != m_pProgress ){
        m_pProgress->setValue(0);
    }
    else
        qCCritical(GUI) << Q_FUNC_INFO << "progressbar not found!";


    // записываем текущий жест в базу данных
    m_gestsDB << m_gestData;

    m_repeat++;
    if ( m_repeat > m_params.m_vectors ){
        m_repeat = 1;
        m_gests.pop_front();
        if ( 0 == m_gests.size() ){
            m_state = LRN_DONE;
            update();
            return;
        }
    }
    m_state = LRN_READY;
    update();
}


void MainDialog::_save(){

    //QString fileName = QFileDialog::getSaveFileName(this, tr("Save data"), "", tr("Data files ( *.raw *.data)"));
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save data"), "", tr(""));
    qCDebug(GUI) << Q_FUNC_INFO << "fileName=" << fileName;

    if(fileName.isEmpty()){
        QMessageBox::critical(this, "SignalSave2", QString::fromUtf8("Не задано имя файла!"));
        qCCritical(GUI) << Q_FUNC_INFO << "file name is empty!";
        return;
    }

    /*
     *  Сохранение сырых данных
     */

    {
        QString rawName= fileName+QString::fromUtf8(".raw");

        QFile File( rawName );
        if ( File.open(QIODevice::WriteOnly | QIODevice::Text )){

            int gest                    = -1;
            int outputs                 = 0;
            int inputs                  = ELECTRODES_AMOUNT;
            int size                    = 0;

            for ( GestData & data : m_gestsDB ){
                size += data.m_counter;
                if ( data.m_gest != gest ){
                    gest = data.m_gest;
                    outputs++;
                }
            }

            QString _header = QString("%1 %2 %3\n").arg(size).arg(inputs).arg(outputs);
            File.write( _header.toLocal8Bit() );

            gest                = -1;
            int class_counter   = -1;
            QString _classline;

            for ( GestData & data : m_gestsDB ){

                qCDebug(GUI) << Q_FUNC_INFO << "number=" << data.m_number;
                qCDebug(GUI) << Q_FUNC_INFO << "counter=" << data.m_counter;
                qCDebug(GUI) << Q_FUNC_INFO << "gest=" << data.m_gest;

                qCDebug(GUI) << Q_FUNC_INFO << "ch0" << data.m_channel[0];
                qCDebug(GUI) << Q_FUNC_INFO << "ch1" << data.m_channel[1];
                qCDebug(GUI) << Q_FUNC_INFO << "ch2" << data.m_channel[2];
                qCDebug(GUI) << Q_FUNC_INFO << "ch3" << data.m_channel[3];
                qCDebug(GUI) << Q_FUNC_INFO << "ch4" << data.m_channel[4];
                qCDebug(GUI) << Q_FUNC_INFO << "ch5" << data.m_channel[5];
                qCDebug(GUI) << Q_FUNC_INFO << "ch6" << data.m_channel[6];
                qCDebug(GUI) << Q_FUNC_INFO << "ch7" << data.m_channel[7];

                QString _line;
                int _counter = 0;

                while ( _counter < data.m_counter ){
                    _line = QString("%1 %2 %3 %4 %5 %6 %7 %8\n")
                            .arg(data.m_channel[0].at(_counter))
                            .arg(data.m_channel[1].at(_counter))
                            .arg(data.m_channel[2].at(_counter))
                            .arg(data.m_channel[3].at(_counter))
                            .arg(data.m_channel[4].at(_counter))
                            .arg(data.m_channel[5].at(_counter))
                            .arg(data.m_channel[6].at(_counter))
                            .arg(data.m_channel[7].at(_counter));

                    File.write( _line.toLocal8Bit() );

                    if ( gest != data.m_gest ){
                        _classline = QString("");
                        gest = data.m_gest;
                        class_counter++;
                        for( int i = 0; i < outputs; i ++ ){
                            if (class_counter == i){
                                _classline += (_classline.length()>0)?(QString(" 1")):(QString("1"));
                            }
                            else {
                                _classline += (_classline.length()>0)?(QString(" 0")):(QString("0"));
                            }
                        }
                        _classline += QString("\n");
                    }
                    File.write( _classline.toLocal8Bit() );

                    _counter++;
                }
            }
            File.close();
        }
        else {
            QMessageBox::critical(this, "SignalSave2", QString::fromUtf8("Ошибка сохранения raw файла!"));
            qCCritical(GUI)  << Q_FUNC_INFO << "can't open raw file";
            return;
        }
    }

    /*
     * Сохранение данных после предобработки для автокодировщика по требованиям Захара
     * --------------------------------------------------------------------------------------
     * вычисление квадрата дисперсии, нормировка, центрирование и сигмоида.
     * для каждого хвата соединяем векторы для N поворений. Производим вычисления на заданном окне, пишем значения полученного
     * вектора (точки в частном случае 8 координатами) в файл, сдвигаем окно на 1 отсчет и снова повтояряем действия.
     */
    {
        QString dataName= fileName+QString::fromUtf8(".data");

        QFile File( dataName );
        if ( File.open(QIODevice::WriteOnly | QIODevice::Text )){

            QStringList lines;

            int gest                    = -1;
            int outputs                 = 0;
            int inputs                  = ELECTRODES_AMOUNT;
            //int size                    = 0;

            for ( GestData & data : m_gestsDB ){
                //size += data.m_counter;
                if ( data.m_gest != gest ){
                    gest = data.m_gest;
                    outputs++;
                }
            }

            QVector<float> _channel[ELECTRODES_AMOUNT]; // объединенный вектор одного жеста для N повторений
            QVector<float> _data[ELECTRODES_AMOUNT];    // данные предобработки
            gest                = -1;
            int class_counter   = -1;

            for ( GestData & data : m_gestsDB ){

                if ( gest != data.m_gest ){

                    // жест сменился, обрабатываем данные
                    if ( _channel[0].size() > 0 ){
                        for ( int i = 0; i < ELECTRODES_AMOUNT; i++ ){
                            int pos = 0;
                            while(1){
                                QVector<float> frame;
                                frame = _channel[i].mid(pos, m_params.m_window);
                                if ( frame.size() < m_params.m_window ) break;
                                float AVG = MATH::average(frame);
                                float DISP= MATH::dispersion(frame, AVG);
                                float SIGM= MATH::sigma( DISP, m_params.m_K, m_params.m_r, m_params.m_Beta );
                                qCDebug(GUI)  << Q_FUNC_INFO << "frame" << frame;
                                qCDebug(GUI)  << Q_FUNC_INFO << "AVG=" << AVG << "DISP=" << DISP << "SIGM=" << SIGM;
                                _data[i] << SIGM;
                                pos++;
                            }
                        }//на выходе _data содержит данные предобработки

                        if ( _data[0].size() > 0 ){
                            for(int pos=0; pos < _data[0].size(); pos++){
                                QString dataline;
                                QString classline;
                                // формируем строку данных
                                for(int i=0; i < ELECTRODES_AMOUNT; i++){
                                    if ( dataline.size() > 0 ){
                                        dataline += QString(" %1").arg(_data[i].at(pos));
                                    }
                                    else {
                                        dataline = QString("%1").arg(_data[i].at(pos));
                                    }
                                }
                                dataline += QString("\n");
                                // формируем строку классов
                                for( int i = 0; i < outputs; i ++ ){
                                    if (class_counter == i){
                                        classline += (classline.length()>0)?(QString(" 1")):(QString("1"));
                                    }
                                    else {
                                        classline += (classline.length()>0)?(QString(" 0")):(QString("0"));
                                    }
                                }
                                classline += QString("\n");

                                lines.push_back( dataline );
                                lines.push_back( classline);
                            }
                        }
                        else {
                            qCCritical(GUI)  << Q_FUNC_INFO << "NO PREPROCESSING DATA-1!!!";
                        }
                    }
                    // записываем новый номер текущего жеста
                    gest = data.m_gest;
                    // увеличиваем счетчик класса
                    class_counter++;
                    // очищаем объединенный вектор и данные предобработки
                    for( int i = 0; i < ELECTRODES_AMOUNT; i++ ) {
                        _channel[i].clear();
                        _channel[i].squeeze();

                        _data[i].clear();
                        _data[i].squeeze();
                    }
                    // объединяем данные следующего жеста
                    for( int i = 0; i < ELECTRODES_AMOUNT; i++ ) _channel[i] << data.m_channel[i];
                }
                else {
                    // объединяем данные текущего жеста
                    for( int i = 0; i < ELECTRODES_AMOUNT; i++ ) _channel[i] << data.m_channel[i];
                }

            }

            if ( _channel[0].size() > 0 ){
                for ( int i = 0; i < ELECTRODES_AMOUNT; i++ ){
                    int pos = 0;
                    while(1){
                        QVector<float> frame;
                        frame = _channel[i].mid(pos, m_params.m_window);
                        if ( frame.size() < m_params.m_window ) break;
                        float AVG = MATH::average(frame);
                        float DISP= MATH::dispersion(frame, AVG);
                        float SIGM= MATH::sigma( DISP, m_params.m_K, m_params.m_r, m_params.m_Beta );
                        qCDebug(GUI)  << Q_FUNC_INFO << "frame" << frame;
                        qCDebug(GUI)  << Q_FUNC_INFO << "AVG=" << AVG << "DISP=" << DISP << "SIGM=" << SIGM;
                        _data[i] << SIGM;
                        pos++;
                    }
                }//на выходе _data содержит данные предобработки

                if ( _data[0].size() > 0 ){
                    for(int pos=0; pos < _data[0].size(); pos++){
                        QString dataline;
                        QString classline;
                        // формируем строку данных
                        for(int i=0; i < ELECTRODES_AMOUNT; i++){
                            if ( dataline.size() > 0 ){
                                dataline += QString(" %1").arg(_data[i].at(pos));
                            }
                            else {
                                dataline = QString("%1").arg(_data[i].at(pos));
                            }
                        }
                        dataline += QString("\n");
                        // формируем строку классов
                        for( int i = 0; i < outputs; i ++ ){
                            if (class_counter == i){
                                classline += (classline.length()>0)?(QString(" 1")):(QString("1"));
                            }
                            else {
                                classline += (classline.length()>0)?(QString(" 0")):(QString("0"));
                            }
                        }
                        classline += QString("\n");

                        lines.push_back( dataline );
                        lines.push_back( classline);
                    }
                }
                else {
                    qCCritical(GUI)  << Q_FUNC_INFO << "NO PREPROCESSING DATA-2!!!";
                }
            }

            int size = lines.size();
            size = size/2;
            QString header = QString("%1 %2 %3\n").arg(size).arg(inputs).arg(outputs);
            lines.push_front(header);
            /// запись
            for ( QString & line : lines ){
                File.write( line.toLocal8Bit() );
            }

            File.close();
        }
        else {
            QMessageBox::critical(this, "SignalSave2", QString::fromUtf8("Ошибка сохранения data файла!"));
            qCCritical(GUI)  << Q_FUNC_INFO << "can't open data file";
            return;
        }
    }
}

void MainDialog::_timeout(){
    qCDebug(GUI) << Q_FUNC_INFO << "m_state=" << m_state;
    if ( LRN_RECORDING == m_state ){

        if ( Q_NULLPTR != m_pProgress ){
            m_pProgress->setValue(0);
        }
        else
            qCCritical(GUI) << Q_FUNC_INFO << "progressbar not found!";

        QMessageBox::critical(this, "SignalSave2", QString::fromUtf8("Нет данных от браслета!"));
        qCCritical(GUI) << Q_FUNC_INFO << "m_state=" << m_state << QString::fromUtf8("Нет данных от браслета!");

        m_state = LRN_NODATA;
        update();
        m_gestsDB.clear();
    }
    else if ( LRN_TESTING == m_state ){
        QMessageBox::critical(this, "SignalSave2", QString::fromUtf8("Нет данных от браслета!"));
        qCCritical(GUI) << Q_FUNC_INFO << "m_state=" << m_state << QString::fromUtf8("Нет данных от браслета!");

        m_state = LRN_DONE;
        update();
    }
}

void MainDialog::update(){
    qCDebug(GUI) << Q_FUNC_INFO;

    auto pbtnStart  = findChild<QPushButton*>( _pbtnStart );
    auto pbtnRecord = findChild<QPushButton*>( _pbtnRecord );
    auto pbtnReset  = findChild<QPushButton*>( _pbtnReset );
    auto pbtnTest   = findChild<QPushButton*>( _pbtnTest );
    auto pbtnStopTst= findChild<QPushButton*>( _pbtnStopTst );
    auto pbtnSave   = findChild<QPushButton*>( _pbtnSave );

    if ( !pbtnStart ){
        qCCritical(GUI) << Q_FUNC_INFO << "can't find start btn!";
        return;
    }
    if ( !pbtnRecord ){
        qCCritical(GUI) << Q_FUNC_INFO << "can't find record btn!";
        return;
    }
    if ( !pbtnReset ){
        qCCritical(GUI) << Q_FUNC_INFO << "can't find reset btn!";
        return;
    }
    if ( !pbtnTest ){
        qCCritical(GUI) << Q_FUNC_INFO << "can't find test btn!";
        return;
    }
    if ( !pbtnStopTst ){
        qCCritical(GUI) << Q_FUNC_INFO << "can't find stop test btn!";
        return;
    }
    if ( !pbtnSave ){
        qCCritical(GUI) << Q_FUNC_INFO << "can't find save btn!";
        return;
    }

    switch (m_state){
    case LRN_NODATA:
        for( auto elem: m_paramsGroup){
            elem->setEnabled( true );
        }
        pbtnStart->setEnabled(true);
        pbtnRecord->setDisabled(true);
        pbtnReset->setDisabled(true);
        pbtnTest->setDisabled(true);
        pbtnStopTst->setDisabled(true);
        pbtnSave->setDisabled(true);
        showNoData();
        break;
    case LRN_READY:
        for( auto elem: m_paramsGroup){
            elem->setDisabled( true );
        }
        pbtnStart->setDisabled( true );
        pbtnRecord->setEnabled( true );
        pbtnReset->setDisabled(true);
        pbtnTest->setDisabled(true);
        pbtnStopTst->setDisabled(true);
        pbtnSave->setDisabled(true);
        showGest( m_gests.at(0) );
        break;
    case LRN_RECORDING:
        for( auto elem: m_paramsGroup){
            elem->setDisabled( true );
        }
        pbtnStart->setDisabled( true );
        pbtnRecord->setDisabled( true );
        pbtnReset->setDisabled(true);
        pbtnTest->setDisabled(true);
        pbtnStopTst->setDisabled(true);
        pbtnSave->setDisabled(true);
        showGest( m_gests.at(0), m_repeat );
        break;
    case LRN_DONE:
        for( auto elem: m_paramsGroup){
            elem->setDisabled( true );
        }
        pbtnStart->setDisabled( true );
        pbtnRecord->setDisabled( true );
        pbtnReset->setEnabled(true);
        pbtnTest->setEnabled(true);
        pbtnStopTst->setEnabled(true);
        pbtnSave->setEnabled(true);
        showDone();
        break;
    case LRN_TESTING:
        for( auto elem: m_paramsGroup){
            elem->setDisabled( true );
        }
        pbtnStart->setDisabled( true );
        pbtnRecord->setDisabled( true );
        pbtnReset->setDisabled(true);
        pbtnTest->setDisabled(true);
        pbtnStopTst->setEnabled(true);
        pbtnSave->setDisabled(true);
        showDone();
        break;
    }
}

void MainDialog::_gest(int gest){
    if ( LRN_TESTING == m_state ){
        if ( (gest < 1 )||(gest > GESTURES_AMOUNT)){
            qCCritical(GUI) << Q_FUNC_INFO << "Bad gest number gest=" << gest;
            return;
        }
        showGest( gest );
    }
}

void MainDialog::showNoData(){
    qCDebug(GUI) << Q_FUNC_INFO;
    auto imageLabel = findChild<QLabel*>( _imageLabel );
    if ( imageLabel ){
        QImage image( 987,692, QImage::Format_ARGB32_Premultiplied);
        image.fill(qRgba(0, 0, 0, 0)); /* прозрачный */
        QPainter painter(&image);
        painter.setPen( QColor(Qt::black) );
        QFont f( "Helvetica",86, QFont::Bold );
        painter.setFont(f);
        //painter.fillRect(image.rect(), Qt::yellow);
        painter.drawText(image.rect(), Qt::AlignCenter | Qt::AlignVCenter, QString::fromUtf8("Данных нет"));
        painter.end();

        imageLabel->setPixmap(QPixmap::fromImage(image));
        imageLabel->adjustSize();
    }
    else
        qCCritical(GUI) << Q_FUNC_INFO << "can't find image label!";
}

void MainDialog::showDone(){
    qCDebug(GUI) << Q_FUNC_INFO;
    auto imageLabel = findChild<QLabel*>( _imageLabel );
    if ( imageLabel ){
        QImage image( 987,692, QImage::Format_ARGB32_Premultiplied);
        image.fill(qRgba(0, 0, 0, 0)); /* прозрачный */
        QPainter painter(&image);
        painter.setPen( QColor(Qt::darkGreen) );
        QFont f( "Helvetica",68, QFont::Bold );
        painter.setFont(f);
        painter.drawText(image.rect(), Qt::AlignCenter | Qt::AlignVCenter, QString::fromUtf8("Данные записаны"));
        painter.end();

        imageLabel->setPixmap(QPixmap::fromImage(image));
        imageLabel->adjustSize();
    }
    else
        qCCritical(GUI) << Q_FUNC_INFO << "can't find image label!";

}

void MainDialog::showGest( const int gest, int repeat ){
    qCDebug(GUI) << Q_FUNC_INFO << "start gest=" << gest;

    if ( (gest < 1) || (gest > GESTURES_AMOUNT) ){
        qCCritical(GUI) << Q_FUNC_INFO << "bad gest number!";
        return;
    }

    auto imageLabel = findChild<QLabel*>( _imageLabel );
    if ( imageLabel ){

        bool leftHand = m_params.m_leftHand;

        QImage image1 = m_listImages.at(gest-1);
        QImage image2 = image1.mirrored( leftHand, false );

        QPainter painter(&image2);
        painter.setPen( QColor(Qt::black) );
        QFont f( "Helvetica",6 );
        painter.setFont(f);
        switch (gest) {
        case 1:
            painter.drawText( leftHand?440:380 ,60,QString::fromUtf8("Расслабление"));
            if ( repeat > 0 ){
                painter.setPen( QColor(Qt::red) );
                painter.drawText(leftHand?460:400,660, QString::fromUtf8("Запись №%1").arg(repeat));
            }
            break;
        case 2:
            painter.drawText(leftHand?470:420,60,QString::fromUtf8("Щепоть"));
            if ( repeat > 0 ){
                painter.setPen( QColor(Qt::red) );
                painter.drawText(leftHand?460:400,660, QString::fromUtf8("Запись №%1").arg(repeat));
            }
            break;
        case 3:
            painter.drawText(leftHand?500:420,60,QString::fromUtf8("Кулак"));
            if ( repeat > 0 ){
                painter.setPen( QColor(Qt::red) );
                painter.drawText(leftHand?460:400,660, QString::fromUtf8("Запись №%1").arg(repeat));
            }
            break;
        case 4:
            painter.drawText( leftHand?510:380,60,QString::fromUtf8("Цилиндр"));
            if ( repeat > 0 ){
                painter.setPen( QColor(Qt::red) );
                painter.drawText(leftHand?490:370,660, QString::fromUtf8("Запись №%1").arg(repeat));
            }
            break;
        case 5:
            painter.drawText( leftHand?440:280,60,QString::fromUtf8("Движение пальца вниз"));
            if ( repeat > 0 ){
                painter.setPen( QColor(Qt::red) );
                painter.drawText(leftHand?490:360,660, QString::fromUtf8("Запись №%1").arg(repeat));
            }
            break;
        case 6:
            painter.drawText(leftHand?380:340,60,QString::fromUtf8("Движение пальца вверх"));
            if ( repeat > 0 ){
                painter.setPen( QColor(Qt::red) );
                painter.drawText(leftHand?440:400,660, QString::fromUtf8("Запись №%1").arg(repeat));
            }
            break;
        case 7:
            painter.drawText(leftHand?470:400,60,QString::fromUtf8("Раскрытие"));
            if ( repeat > 0 ){
                painter.setPen( QColor(Qt::red) );
                painter.drawText(leftHand?470:400,660, QString::fromUtf8("Запись №%1").arg(repeat));
            }
            break;
        case 8:
            painter.drawText(leftHand?520:400,60,QString::fromUtf8("Класс"));
            if ( repeat > 0 ){
                painter.setPen( QColor(Qt::red) );
                painter.drawText(leftHand?490:380,660, QString::fromUtf8("Запись №%1").arg(repeat));
            }
            break;
        case 9:
            painter.drawText(leftHand?450:390,60,QString::fromUtf8("Приветствие"));
            if ( repeat > 0 ){
                painter.setPen( QColor(Qt::red) );
                painter.drawText(leftHand?470:400,660, QString::fromUtf8("Запись №%1").arg(repeat));
            }
            break;
        case 10:
            painter.drawText(leftHand?480:420,60,QString::fromUtf8("Победа"));
            if ( repeat > 0 ){
                painter.setPen( QColor(Qt::red) );
                painter.drawText(leftHand?460:400,660, QString::fromUtf8("Запись №%1").arg(repeat));
            }
            break;
        case 11: {
            QFont ff( "Helvetica",18 );
            painter.setFont(ff);
            painter.drawText(leftHand?400:400,60,QString::fromUtf8("Поворот от себя"));
            if ( repeat > 0 ){
                painter.setPen( QColor(Qt::red) );
                painter.drawText(leftHand?430:430,660, QString::fromUtf8("Запись №%1").arg(repeat));
            }
            break;}
        case 12:{
            QFont ff( "Helvetica",18 );
            painter.setFont(ff);
            painter.drawText(leftHand?400:380,60,QString::fromUtf8("Поворот на себя"));
            if ( repeat > 0 ){
                painter.setPen( QColor(Qt::red) );
                painter.drawText(leftHand?430:430,660, QString::fromUtf8("Запись №%1").arg(repeat));
            }
            break;}
        case 13:{
            QFont ff( "Helvetica",18 );
            painter.setFont(ff);
            painter.drawText(320,60,QString::fromUtf8("Вращение по часовой стрелке"));
            if ( repeat > 0 ){
                painter.setPen( QColor(Qt::red) );
                painter.drawText(leftHand?430:430,660, QString::fromUtf8("Запись №%1").arg(repeat));
            }
            break;}
        case 14:{
            QFont ff( "Helvetica",18 );
            painter.setFont(ff);
            painter.drawText(leftHand?300:300,60,QString::fromUtf8("Вращение против часовой стрелки"));
            if ( repeat > 0 ){
                painter.setPen( QColor(Qt::red) );
                painter.drawText(leftHand?420:420,660, QString::fromUtf8("Запись №%1").arg(repeat));
            }
            break;}
        }
        painter.end();

        imageLabel->setScaledContents(true);
         imageLabel->setPixmap(QPixmap::fromImage(image2));
        imageLabel->adjustSize();
    }
    else
        qCCritical(GUI) << Q_FUNC_INFO << "can't find image label!";

    qCDebug(GUI) << Q_FUNC_INFO << "end";
}

/*  ----------------- слоты для Myo браслета ----------------------------- */

void MainDialog::_connectMYO(){
    qCDebug(GUI) << Q_FUNC_INFO;

    if ( !m_myo.on( m_params.m_gain ) ) qCCritical(GUI) << Q_FUNC_INFO << "can't turn on MYO!";
}

void MainDialog::_disconnectMYO(){
    qCDebug(GUI) << Q_FUNC_INFO;
    m_myo.off();
    printStatus(QString::fromUtf8("Готов"));
}

void MainDialog::_errorMYO( const QString & msg ){
    qCDebug(GUI) << Q_FUNC_INFO << msg;

    QMessageBox::critical( this, QString::fromUtf8("Ошибка"), msg );

    auto myoPortWidget  = findChild<portWidget*>(_myoPortWidget);
    if ( myoPortWidget )
        myoPortWidget->off();
    else
        qCCritical(GUI) << Q_FUNC_INFO << "can't find myoPortWidget!";
    printStatus(QString::fromUtf8("Готов"));
}

void MainDialog::_stateMYO(int state ){
    qCDebug(GUI) << Q_FUNC_INFO << "state=" << state;

    MYO::state_t _state = (MYO::state_t)state;

    switch ( _state ){
    case MYO::Undef:
        printStatus(QString::fromUtf8("MYO undef"));
        break;
    case MYO::Connect:
        printStatus(QString::fromUtf8("MYO подключен"));
        break;
    case MYO::Disconnect:
        printStatus(QString::fromUtf8("MYO отключен"));
        break;
    case MYO::ArmSync:
        printStatus(QString::fromUtf8("MYO синхронизирован"));
        break;
    case MYO::ArmUnsync:
        printStatus(QString::fromUtf8("MYO рассинхронизирован"));
        break;
    case MYO::Lock:
        printStatus(QString::fromUtf8("MYO заблокирован"));
        break;
    case MYO::Unlock:
        printStatus(QString::fromUtf8("MYO разблокирован"));
        break;
    case MYO::Pair:
        printStatus(QString::fromUtf8("MYO pair"));
        break;
    case MYO::Unpair:
        printStatus(QString::fromUtf8("MYO unpair"));
        break;
    case MYO::WarmupCompleted:
        printStatus(QString::fromUtf8("MYO нагрет"));
        break;
    default:
        qCCritical(GUI) << Q_FUNC_INFO << "undefined state!";
    }
}

void MainDialog::_packet( MCU::SDU_MYOGRAPH_DATA packet ){

    m_sensors.at(0)->setText(QString::number(packet.m_data[0]));
    m_sensors.at(1)->setText(QString::number(packet.m_data[1]));
    m_sensors.at(2)->setText(QString::number(packet.m_data[2]));
    m_sensors.at(3)->setText(QString::number(packet.m_data[3]));
    m_sensors.at(4)->setText(QString::number(packet.m_data[4]));
    m_sensors.at(5)->setText(QString::number(packet.m_data[5]));
    m_sensors.at(6)->setText(QString::number(packet.m_data[6]));
    m_sensors.at(7)->setText(QString::number(packet.m_data[7]));

    if ( LRN_RECORDING == m_state ) {
        qCDebug(GUI) << Q_FUNC_INFO << packet;
        if ( Q_NULLPTR != m_pTimer ){
            m_pTimer->stop();
            // увеличиваем счетчик
            m_gestData.m_counter++;
            // отборажаем на прогрессбаре
            if ( Q_NULLPTR != m_pProgress ){
                m_pProgress->setValue(m_gestData.m_counter);
            }
            else
                qCCritical(GUI) << Q_FUNC_INFO << "progressbar not found!";

            // записываем данные из пакета
            for ( int i = 0; i < ELECTRODES_AMOUNT; i++ ){
                m_gestData.m_channel[i] << packet.m_data[i];
            }
            // если счетчик больше размера окна, останавливаем запись
            if ( m_gestData.m_counter >= m_params.m_window ){
                emit _stopRecord();
            }
            // иначе, запускаем таймер снова
            else
                m_pTimer->start();
        }
        else {
            qCCritical(GUI) << Q_FUNC_INFO << "timer not found!";
            m_state = LRN_NODATA;
            update();
            m_gestsDB.clear();
            QMessageBox::critical(this, "SignalSave2", QString::fromUtf8("Критическая ошибка, нет таймера!"));
        }
    }
    else if ( LRN_TESTING == m_state ){
        if ( Q_NULLPTR != m_pTimer ){
            m_pTimer->stop();
            m_pTimer->start();
        }
        else {
            qCCritical(GUI) << Q_FUNC_INFO << "timer not found!";
            m_state = LRN_NODATA;
            update();
            m_gestsDB.clear();
            QMessageBox::critical(this, "SignalSave2", QString::fromUtf8("Критическая ошибка, нет таймера!"));
            return;
        }
    }
}
/// ------------ слоты для MCU -------------------------------------

void MainDialog::_connectMCU(){
    qCDebug(GUI) << Q_FUNC_INFO;
    auto myoPortWidget  = findChild<portWidget*>(_myoPortWidget);
    if ( myoPortWidget ){
        qint32 baudRate = myoPortWidget->getBaudRate();
        QString portName= myoPortWidget->getPortName();
        m_mcu.on( portName, baudRate, m_params.m_gain );
    }
    else
        qCCritical(GUI) << Q_FUNC_INFO << "can't find myoPortWidget!";
}

void MainDialog::_disconnectMCU(){
    qCDebug(GUI) << Q_FUNC_INFO;
    m_mcu.off();
    printStatus(QString::fromUtf8("Готов"));
}

void MainDialog::_state (const QString & msg ){
    qCDebug(GUI) << Q_FUNC_INFO << msg;
    printStatus(msg);
}


////////////////////////////////////////////////////////////////////////////
/// реализация portWidget

#include <QSerialPortInfo>

portWidget::portWidget(QString objectName, QWidget *parent, bool bMyo):
    QWidget(parent), m_bMyo(bMyo) {

    this->setObjectName( objectName );

    qCDebug(GUI) << Q_FUNC_INFO  << this->objectName() ;
    // размещение
    auto mainLayout = new QVBoxLayout;
    setLayout( mainLayout );
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    auto grid = new QGridLayout();
    mainLayout->addLayout( grid );

    // ~~~~ панель компорта ~~~~
    auto portLabel  = new QLabel( QString::fromUtf8("Порт:") );
    m_portCombo  = new QComboBox();
    grid->addWidget( portLabel, 0, 0 );
    grid->addWidget( m_portCombo, 0, 1 );

    // ~~~~ панель скорости ~~~~
    auto speedLabel = new QLabel( QString::fromUtf8("Скорость:") );
    m_speedCombo    = new QComboBox();
    grid->addWidget( speedLabel, 1, 0 );
    grid->addWidget( m_speedCombo, 1, 1 );

    // ~~~~ панель кнопок ~~~~
    m_onoffBtn      = new QPushButton( QIcon(QString::fromUtf8(":/off")),"", this );
    m_onoffBtn->setMaximumSize ( m_onoffBtn->sizeHint().height(), m_onoffBtn->sizeHint().height() );
    m_onoffBtn->setToolTip(QString::fromUtf8("Подключить"));

    grid->addWidget( m_onoffBtn, 2, 0 );

    if ( m_bMyo ){
        m_myoCheckBox = new QCheckBox(QString("Myo"), this );
        grid->addWidget( m_myoCheckBox, 2, 1 );

        connect( m_myoCheckBox, &QCheckBox::stateChanged, this, &portWidget::chkbxChanged );
    }

    grid->addWidget( new QLabel( QString::fromUtf8("")), 3, 0 );
    grid->addWidget( new QLabel( QString::fromUtf8("")), 3, 1 );


    // ~~~~ connections ~~~~
    connect( m_onoffBtn,    &QPushButton::clicked,              this,	&portWidget::OnOff);
    connect( m_portCombo,   &QComboBox::currentTextChanged,     this,   &portWidget::portChanged );
    connect( m_speedCombo,  &QComboBox::currentTextChanged,     this,   &portWidget::speedChanged );

    // ~~~~ init com-combo ~~~~
    qCDebug(GUI) << Q_FUNC_INFO  << this->objectName() << "find ports for Port Combo...";
    foreach ( auto &info, QSerialPortInfo::availablePorts()) {
        QString name = info.portName();
        qCDebug(GUI) << Q_FUNC_INFO  << this->objectName() << name << " found";
        m_portCombo->addItem( name );
    }
    if ( m_portCombo->count() ) m_portName = m_portCombo->currentText();

    // ~~~~ init speedcombo ~~~~
    qCDebug(GUI) << Q_FUNC_INFO  << this->objectName() << "find rates for Speed Combo...";
    foreach( auto baudRate, QSerialPortInfo::standardBaudRates()){
        qCDebug(GUI) << Q_FUNC_INFO  << this->objectName() << baudRate << " found";
        m_speedCombo->addItem( QString::number( baudRate ) , QVariant::fromValue(baudRate) );
    }
    m_speedCombo->addItem( "460800"  , QVariant::fromValue(460800) );
    m_speedCombo->addItem( "512000"  , QVariant::fromValue(512000) );
    m_speedCombo->addItem( "921600"  , QVariant::fromValue(921600) );
    m_speedCombo->addItem( "1000000" , QVariant::fromValue(1000000));

    if ( m_speedCombo->count() ) m_baudRate = m_speedCombo->currentData().toInt();
}

portWidget::~portWidget(){
    qCDebug(GUI) << Q_FUNC_INFO << objectName();
}

void portWidget::chkbxChanged(int state){
    qCDebug(GUI) << Q_FUNC_INFO << objectName();
    switch (state){
    case Qt::Unchecked:
        if ( m_bMyo ){
            if ( Q_NULLPTR != m_portCombo ) m_portCombo->setEnabled(true);
            if ( Q_NULLPTR != m_speedCombo ) m_speedCombo->setEnabled(true);
        }
        break;
    case Qt::PartiallyChecked:
        // unused
        break;
    case Qt::Checked:
        if (m_bMyo){
            if ( Q_NULLPTR != m_portCombo ) m_portCombo->setDisabled(true);
            if ( Q_NULLPTR != m_speedCombo ) m_speedCombo->setDisabled(true);
        }
        break;
    }
}

void portWidget::OnOff(){
    qCDebug(GUI) << Q_FUNC_INFO << objectName();

    if ( m_enabled ){
        m_enabled = false;
        if ( Q_NULLPTR != m_onoffBtn ) {
            m_onoffBtn->setIcon(QIcon(QString::fromUtf8(":/off")));
            m_onoffBtn->setToolTip(QString::fromUtf8("Подключить"));
        }

        if ( Q_NULLPTR != m_portCombo ) m_portCombo->setEnabled(true);
        if ( Q_NULLPTR != m_speedCombo ) m_speedCombo->setEnabled(true);

        if (m_bMyo){
            if ( Q_NULLPTR != m_myoCheckBox ) m_myoCheckBox->setEnabled(true);
        }

        if ( Q_NULLPTR != m_myoCheckBox ){
            if (m_myoCheckBox->isChecked())
                emit s_offMyo();
            else
                emit s_off();
        }
    }
    else {
        m_enabled = true;
        if ( Q_NULLPTR != m_onoffBtn ) {
            m_onoffBtn->setIcon(QIcon(QString::fromUtf8(":/on")));
            m_onoffBtn->setToolTip(QString::fromUtf8("Отключить"));
        }

        if ( Q_NULLPTR != m_portCombo ) m_portCombo->setDisabled(true);
        if ( Q_NULLPTR != m_speedCombo ) m_speedCombo->setDisabled(true);

        if (m_bMyo){
            if ( Q_NULLPTR != m_myoCheckBox ) m_myoCheckBox->setDisabled(true);
        }

        if ( Q_NULLPTR != m_myoCheckBox ){
            if (m_myoCheckBox->isChecked())
                emit s_onMyo();
            else
                emit s_on();
        }
    }
}

void portWidget::off(){
    qCDebug(GUI) << Q_FUNC_INFO << objectName();

    if ( m_enabled ){
        OnOff();
    }
}


void portWidget::disableMyo(){
    qCDebug(GUI) << Q_FUNC_INFO;
    if (m_bMyo){
        if ( Q_NULLPTR != m_myoCheckBox ){
            if (m_myoCheckBox->isChecked()) m_myoCheckBox->setChecked(false);
            m_myoCheckBox->hide();
        }
    }
}

void portWidget::enableMyo(){
    qCDebug(GUI) << Q_FUNC_INFO;
    if (m_bMyo){
        if ( Q_NULLPTR != m_myoCheckBox ){
            m_myoCheckBox->show();
        }
    }
}

void portWidget::portChanged(const QString & port ){
    qCDebug(GUI) << Q_FUNC_INFO << objectName() << "port=" << port;
    m_portName = port;
}

void portWidget::speedChanged( const QString & speed ){
    qCDebug(GUI) << Q_FUNC_INFO << objectName() << "speed=" << speed;
    bool ok;
    int _speed = speed.toInt( &ok );
    if ( ok ) m_baudRate = _speed;
}

QString portWidget::getPortName(){
    qCDebug(GUI) << Q_FUNC_INFO << objectName() << "m_portName=" << m_portName;
    return (m_portName);
}

qint32 portWidget::getBaudRate(){
    qCDebug(GUI) << Q_FUNC_INFO << objectName() << "m_baudRate=" << m_baudRate;
    return(m_baudRate);
}


