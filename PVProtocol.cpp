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
                           QString::number(byteArrayToInt(ba.mid(1, 2))));
        itsReadData.insert(QString(DATA_PASCAL), QString::number(byteArrayToInt(ba.mid(3, 4))));
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

// TODO fix overflow int, then int > 32768
int PVProtocol::byteArrayToInt(const QByteArray &ba)
{
    int result = 0;

    if(ba.size() > static_cast<int>(sizeof(int))) {
        return -1;
    }

    for(int i = 0; i < ba.size(); ++i) {
        result += (ba.at(i) << (ba.size() - 1 - i)*8) & (0xFF << (ba.size() - 1 - i)*8);
    }

    return result;
}
