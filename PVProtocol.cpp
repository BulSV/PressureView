#include "PVProtocol.h"

#ifdef DEBUG
#include <QDebug>
#endif

#define STARTBYTE 0x55
#define STOPBYTE 0xAA
#define BYTESLENTH 8

#define DATA_VOLT "VOLT"
#define DATA_PASCAL "PASC"

PVProtocol::PVProtocol(ComPort *comPort, QObject *parent) :
    IProtocol(parent),
    itsComPort(comPort)
{
    connect(itsComPort, SIGNAL(DataIsReaded(bool)), this, SLOT(readData(bool)));
}

void PVProtocol::setDataToWrite(const QMultiMap<QString, QString> &data)
{
}

QMultiMap<QString, QString> PVProtocol::getReadedData() const
{
    return itsReadData;
}

void PVProtocol::readData(bool isReaded)
{
    itsReadData.clear();

    if(isReaded) {
        QByteArray ba;

        ba = itsComPort->getReadData();
#ifdef DEBUG
        qDebug() << "read: " << QString::number(static_cast<int>(ba.at(1)));
        for(int i = 0; i < ba.size(); ++i) {
            qDebug() << "ba =" << (int)ba.at(i);
        }
#endif
        itsReadData.insert(QString(DATA_VOLT),
                           QString::number(wordToInt(ba.mid(1, 2))));
        itsReadData.insert(QString(DATA_PASCAL), QString::number(wordToInt(ba.mid(3, 4))));
        emit DataIsReaded(true);
    } else {
        emit DataIsReaded(false);
    }
}

void PVProtocol::writeData()
{
}

void PVProtocol::resetProtocol()
{
}

// преобразует word в byte
int PVProtocol::wordToInt(QByteArray ba)
{
    if(ba.size() != 2)
        return -1;

    int temp = ba[0];
    if(temp < 0)
    {
        temp += 0x100; // 256;
        temp *= 0x100;
    }
    else
        temp = ba[0]*0x100; // старший байт

    int i = ba[1];
    if(i < 0)
    {
        i += 0x100; // 256;
        temp += i;
    }
    else
        temp += ba[1]; // младший байт

    return temp;
}
