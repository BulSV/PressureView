#include "Dialog.h"
#include <QGridLayout>
#include <QApplication>
#include <QFile>
#include <QShortcut>
#include <QSerialPortInfo>
#include <QPalette>
#include <QIcon>

#define STARTBYTE 0x55
#define STOPBYTE 0xAA
#define BYTESLENTH 8

#define FORMAT 'f'
#define PRECISION 3

#define DIGIT_RANGE_VOLT 7
#define DIGIT_RANGE_PASC 7

#define BLINKTIMETX 200 // ms
#define BLINKTIMERX 500 // ms
#define DISPLAYTIME 100 // ms

#define DATA_VOLT "VOLT"
#define DATA_PASCAL "PASC"

Dialog::Dialog(QWidget *parent) :
        QDialog(parent),
        lPort(new QLabel(QString::fromUtf8("Port"), this)),
        cbPort(new QComboBox(this)),
        lBaud(new QLabel(QString::fromUtf8("Baud"), this)),
        cbBaud(new QComboBox(this)),
        bPortStart(new QPushButton(QString::fromUtf8("Start"), this)),
        bPortStop(new QPushButton(QString::fromUtf8("Stop"), this)),
        lRx(new QLabel("  Rx  ", this)),
        lcdVolt(new QLCDNumber(this)),
        lcdPascal(new QLCDNumber(this)),
        gbVolt(new QGroupBox(QString::fromUtf8("Pressure, V"), this)),
        gbPascal(new QGroupBox(QString::fromUtf8("Pressure, kPa"), this)),
        itsPort(new QSerialPort(this)),
        itsComPort(new ComPort(itsPort, STARTBYTE, STOPBYTE, BYTESLENTH, this)),
        itsProtocol(new PVProtocol(itsComPort, this)),
        itsStatusBar (new QStatusBar(this)),
        itsBlinkTimeRxNone(new QTimer(this)),
        itsBlinkTimeRxColor(new QTimer(this)),
        itsTimeToDisplay(new QTimer(this))
{
    QGridLayout *mainLayout = new QGridLayout;
    setLayout(mainLayout);

    lRx->setStyleSheet("background: red; font: bold; font-size: 10pt");
    lRx->setFrameStyle(QFrame::Box);
    lRx->setAlignment(Qt::AlignCenter);
    lRx->setMargin(2);

    QFrame *frame1 = new QFrame(this);
    frame1->setFrameStyle(QFrame::Box | QFrame::Raised);

    QGridLayout *gridVolt = new QGridLayout;
    gridVolt->addWidget(lcdVolt, 0, 0);
    gridVolt->addWidget(frame1, 0, 0);
    gridVolt->setSpacing(5);

    gbVolt->setLayout(gridVolt);

    QFrame *frame2 = new QFrame(this);
    frame2->setFrameStyle(QFrame::Box | QFrame::Raised);

    QGridLayout *gridPascal = new QGridLayout;
    gridPascal->addWidget(lcdPascal, 0, 1);
    gridPascal->addWidget(frame2, 0, 1);
    gridPascal->setSpacing(5);

    gbPascal->setLayout(gridPascal);

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(lPort, 0, 0);
    grid->addWidget(cbPort, 0, 1);
    grid->addWidget(lBaud, 1, 0);
    grid->addWidget(cbBaud, 1, 1);
    // пещаю логотип фирмы
    grid->addWidget(new QLabel("<img src=':/Resources/elisat.png' height='40' width='150'/>", this), 0, 2, 2, 4, Qt::AlignRight);
    grid->addWidget(bPortStart, 2, 1);
    grid->addWidget(bPortStop, 2, 2);
    grid->addWidget(lRx, 2, 5, Qt::AlignRight);
    grid->setSpacing(5);

    mainLayout->addItem(grid, 0, 0, 3, 6);
    mainLayout->addWidget(gbVolt, 3, 0, 1, 3);
    mainLayout->addWidget(gbPascal, 3, 3, 1, 3);
    mainLayout->addWidget(itsStatusBar, 4, 0, 1, 6);
    mainLayout->setSpacing(5);

    // делает окно фиксированного размера
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    QStringList portsNames;

    foreach(QSerialPortInfo portsAvailable, QSerialPortInfo::availablePorts())
    {
        portsNames << portsAvailable.portName();
    }

    cbPort->addItems(portsNames);
#if defined (Q_OS_LINUX)
    cbPort->setEditable(true); // TODO Make correct viewing available ports in Linux
#else
    cbPort->setEditable(false);
#endif

    QStringList portsBauds;
    portsBauds << "115200" << "57600" << "38400";
    cbBaud->addItems(portsBauds);
    cbBaud->setEditable(false);
    bPortStop->setEnabled(false);

    itsStatusBar->show();

    itsBlinkTimeRxNone->setInterval(BLINKTIMERX);
    itsBlinkTimeRxColor->setInterval(BLINKTIMERX);
    itsTimeToDisplay->setInterval(DISPLAYTIME);

    QList<QLCDNumber*> list;
    list << lcdVolt << lcdPascal;
    foreach(QLCDNumber *lcd, list) {
        lcd->setMinimumSize(80, 25);
        lcd->setMaximumSize(80, 25);
        lcd->setSegmentStyle(QLCDNumber::Flat);
        lcd->setFrameStyle(QFrame::NoFrame);
    }

    lcdVolt->setDigitCount(DIGIT_RANGE_VOLT);
    lcdPascal->setDigitCount(DIGIT_RANGE_PASC);

    connect(bPortStart, SIGNAL(clicked()), this, SLOT(openPort()));
    connect(bPortStop, SIGNAL(clicked()), this, SLOT(closePort()));

    connect(cbPort, SIGNAL(currentIndexChanged(int)), this, SLOT(closePort()));
    connect(cbBaud, SIGNAL(currentIndexChanged(int)), this, SLOT(closePort()));

    connect(itsProtocol, SIGNAL(DataIsReaded(bool)), this, SLOT(received(bool)));

    connect(itsBlinkTimeRxColor, SIGNAL(timeout()), this, SLOT(colorIsRx()));
    connect(itsBlinkTimeRxNone, SIGNAL(timeout()), this, SLOT(colorRxNone()));

    connect(itsTimeToDisplay, SIGNAL(timeout()), this, SLOT(display()));
    connect(itsTimeToDisplay, SIGNAL(timeout()), this, SLOT(display()));

    QShortcut *aboutShortcut = new QShortcut(QKeySequence("F1"), this);
    connect(aboutShortcut, SIGNAL(activated()), qApp, SLOT(aboutQt()));
}

Dialog::~Dialog()
{
    itsPort->close();
}

void Dialog::openPort()
{
    itsPort->close();
    itsPort->setPortName(cbPort->currentText());

    if(itsPort->open(QSerialPort::ReadWrite))
    {
        switch (cbBaud->currentIndex()) {
        case 0:
            itsPort->setBaudRate(QSerialPort::Baud115200);
            break;
        case 1:
            itsPort->setBaudRate(QSerialPort::Baud57600);
            break;
        case 2:
            itsPort->setBaudRate(QSerialPort::Baud38400);
            break;
        default:
            itsPort->setBaudRate(QSerialPort::Baud115200);
            break;
        }

        itsPort->setDataBits(QSerialPort::Data8);
        itsPort->setParity(QSerialPort::NoParity);
        itsPort->setFlowControl(QSerialPort::NoFlowControl);

        itsStatusBar->showMessage(QString::fromUtf8("Port: ") +
                             QString(itsPort->portName()) +
                             QString::fromUtf8(" | Baud: ") +
                             QString(QString::number(itsPort->baudRate())) +
                             QString::fromUtf8(" | Data bits: ") +
                             QString(QString::number(itsPort->dataBits())));
        bPortStart->setEnabled(false);
        bPortStop->setEnabled(true);
        lRx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    }
    else
    {
        itsStatusBar->showMessage(QString::fromUtf8("Error opening port: ") +
                             QString(itsPort->portName()));
        lRx->setStyleSheet("background: red; font: bold; font-size: 10pt");
    }
}

void Dialog::closePort()
{
    itsPort->close();
    itsBlinkTimeRxNone->stop();
    itsBlinkTimeRxColor->stop();
    lRx->setStyleSheet("background: red; font: bold; font-size: 10pt");
    bPortStop->setEnabled(false);
    bPortStart->setEnabled(true);
    itsProtocol->resetProtocol();
}

void Dialog::received(bool isReceived)
{
    if(isReceived) {
        if(!itsBlinkTimeRxColor->isActive() && !itsBlinkTimeRxNone->isActive()) {
            itsBlinkTimeRxColor->start();
            lRx->setStyleSheet("background: green; font: bold; font-size: 10pt");
        }

        if(!itsTimeToDisplay->isActive()) {
            itsTimeToDisplay->start();
        }

        QList<QString> strKeysList = itsProtocol->getReadedData().keys();
        for(int i = 0; i < itsProtocol->getReadedData().size(); ++i) {
            if(strKeysList.at(i) == QString(DATA_VOLT))
            {
                itsDataList.append(addTrailingZeros(QString::number(
                                                        mVtoV(itsProtocol->getReadedData().value(strKeysList.at(i)).toInt())),
                                                    PRECISION));
            } else if(strKeysList.at(i) == QString(DATA_PASCAL)) {
                itsDataList.append(addTrailingZeros(QString::number(
                                                        mVtoV(itsProtocol->getReadedData().value(strKeysList.at(i)).toInt())),
                                                    PRECISION));
#ifdef DEBUG
                qDebug() << "QString::number(DATA):" << itsProtocol->getReadedData().value(strKeysList.at(i));
#endif
            }
        }
    }
}

QString Dialog::addTrailingZeros(QString str, int prec)
{
    if(str.isEmpty() || prec < 1) { // if prec == 0 then it's no sense
        return str;
    }

    int pointIndex = str.indexOf(".");
    if(pointIndex == -1) {
        str.append(".");
        pointIndex = str.size() - 1;
    }

    if(str.size() - 1 - pointIndex < prec) {
        for(int i = 0; i < prec - (str.size() - 1 - pointIndex); ++i) {
            str.append("0");
        }
    }

    return str;
}

float Dialog::mVtoV(const int &mV)
{
#ifdef DEBUG
    qDebug() << "mV:" << mV;
#endif
    if(mV >= 0) {
        return (mV*1000);
    } else {
        return -1;
    }
}

float Dialog::PaTokPa(const int &Pa)
{
#ifdef DEBUG
    qDebug() << "Pa:" << Pa;
#endif
    if(Pa >= 0) {
        return (Pa*0.001);
    } else {
        return -1;
    }
}

void Dialog::colorIsRx()
{
    lRx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    itsBlinkTimeRxColor->stop();
    itsBlinkTimeRxNone->start();
}

void Dialog::colorRxNone()
{
    itsBlinkTimeRxNone->stop();
}

void Dialog::display()
{
#ifdef DEBUG
    qDebug() << "void Dialog::display()";
#endif
    itsTimeToDisplay->stop();

    QList<QLCDNumber*> list;
    list << lcdPascal << lcdVolt;
    QString tempStr;
#ifdef DEBUG
        qDebug() << "itsDataList.size() =" << itsDataList.size();
#endif
    for(int k = 0; k < list.size() && k < itsDataList.size(); ++k) {
        tempStr = itsDataList.at(itsDataList.size() - 1 - k);

        if(list.at(k)->digitCount() < tempStr.size())
        {
            list[k]->display("ERR"); // Overflow
        } else {
            list[k]->display(tempStr);
        }
#ifdef DEBUG
        qDebug() << "itsDataList.size() =" << itsDataList.size();
        qDebug() << "Data[" << k << "] =" << list.at(k)->value();
#endif
    }

    itsDataList.clear();
}
