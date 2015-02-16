#ifndef READSENSORPROTOCOL_H
#define READSENSORPROTOCOL_H

#include "IProtocol.h"
#include "ComPort.h"

class PVProtocol : public IProtocol
{
    Q_OBJECT
public:
    explicit PVProtocol(ComPort *comPort, QObject *parent = 0);
    virtual void setDataToWrite(const QMultiMap<QString, QString> &data);
    virtual QMultiMap<QString, QString> getReadedData() const;
signals:

public slots:
    virtual void writeData();
    virtual void resetProtocol();
private slots:
    void readData(bool isReaded);
private:
    ComPort *itsComPort;

    QMultiMap<QString, QString> itsReadData;

    // преобразует word в byte
    int wordToInt(QByteArray ba);
};

#endif // READSENSORPROTOCOL_H
