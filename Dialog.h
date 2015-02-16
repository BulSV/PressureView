#ifndef DIALOG_H
#define DIALOG_H

#ifdef DEBUG
#include <QDebug>
#endif

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>
#include <QStatusBar>
#include <QTimer>
#include <QLCDNumber>
#include "ComPort.h"
#include "IProtocol.h"
#include "PVProtocol.h"

class Dialog : public QDialog
{
    Q_OBJECT

    enum DATA {
        VOLT,
        PASCAL
    };

    QLabel *lPort;
    QComboBox *cbPort;
    QLabel *lBaud;
    QComboBox *cbBaud;
    QPushButton *bPortStart;
    QPushButton *bPortStop;
    QLabel *lRx;

    QLCDNumber *lcdVolt;
    QLCDNumber *lcdPascal;

    QGroupBox *gbVolt;
    QGroupBox *gbPascal;

    QSerialPort *itsPort;
    ComPort *itsComPort;
    IProtocol *itsProtocol;

    QStringList itsDataList;

    // добавляет завершающие нули
    QString addTrailingZeros(QString str, int prec);

    float mVtoV(const int &mV);
    float PaTokPa(const int &Pa);

    QStatusBar *itsStatusBar;

    QTimer *itsBlinkTimeRxNone;
    QTimer *itsBlinkTimeRxColor;
    QTimer *itsTimeToDisplay;

private slots:
    void openPort();
    void closePort();
    void received(bool isReceived);
    // мигание надписей "Rx" - при получении и "Tx" - при отправке пакета
    void colorRxNone();
    void colorIsRx();
    // display current Rx data
    void display();
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
};

#endif // DIALOG_H
