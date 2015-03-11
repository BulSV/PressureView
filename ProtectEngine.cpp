#include <QMessageBox>
#include <QDataStream>
#include "ProtectEngine.h"

#ifdef DEBUG
#include <QDebug>
#endif

ProtectEngine::ProtectEngine(IProtectedProgram *protectedProgram,
                             const QString &licenseFileName)
    : m_ProtectedProgram(protectedProgram)
    , m_LicenseFile(new QFile(licenseFileName))
    , m_CurrentDate(new QDate)
{
}

ProtectEngine::~ProtectEngine()
{
}

void ProtectEngine::protect()
{
    if(!isValidLicense()) {
        QMessageBox::critical(0, "Start Program Failed", "Run Time Error"/*"File " + m_LicenseFile->fileName() + " is corrupt"*/);
        m_LicenseFile->close();
        writeLastTimeRun();
        throw "Renew license!";
    } else {
        m_LicenseFile->close();
        m_ProtectedProgram->startProgram();
    }

    writeLastTimeRun();
}

bool ProtectEngine::isValidLicense()
{
    if(m_LicenseFile->open(QFile::ReadOnly)) {
        m_CurrentDate->setDate(QDate::currentDate().year(),
                               QDate::currentDate().month(),
                               QDate::currentDate().day());
#ifdef DEBUG
        qDebug() << "CURRENT: bool ProtectEngine::isValidLicense()";
        qDebug() << m_CurrentDate->day();
        qDebug() << m_CurrentDate->month();
        qDebug() << m_CurrentDate->year();
#endif
        QDataStream stream(m_LicenseFile);
        stream.setVersion(QDataStream::Qt_5_2);
        QDate startDate;
        QDate endDate;
        QDate lastDate;

        stream >> startDate;
#ifdef DEBUG
        if(stream.status() != QDataStream::Ok)
        {
            qDebug() << "start: Ошибка чтения файла" << stream.status();
        }
#endif
        stream >> endDate;
#ifdef DEBUG
        if(stream.status() != QDataStream::Ok)
        {
            qDebug() << "end: Ошибка чтения файла" << stream.status();
        }
#endif
        stream >> lastDate;
#ifdef DEBUG
        if(stream.status() != QDataStream::Ok)
        {
            qDebug() << "last: Ошибка чтения файла" << stream.status();
        }
#endif
#ifdef DEBUG
        qDebug() << "READ: bool ProtectEngine::isValidLicense()";
        qDebug() << "Start Time:";
        qDebug() << startDate;
        qDebug() << "End Time:";
        qDebug() << endDate;
        qDebug() << "Last Time:";
        qDebug() << lastDate;
#endif
        if(startDate <= *m_CurrentDate
                && endDate >= *m_CurrentDate
                && lastDate <= *m_CurrentDate) {
#ifdef DEBUG
            qDebug() << "TRUE";
#endif
            return true;
        }
    }
#ifdef DEBUG
            qDebug() << "FALSE";
#endif
    return false;
}

void ProtectEngine::writeLastTimeRun()
{
#ifdef DEBUG
        qDebug() << "void ProtectEngine::writeLastTimeRun()";
#endif
    if(m_LicenseFile->open(QFile::ReadWrite)) {
        QDataStream stream(m_LicenseFile);
        stream.setVersion(QDataStream::Qt_5_2);

        stream.device()->seek(static_cast<qint64>(2*sizeof(QDate)));
        stream << *m_CurrentDate;

        if(stream.status() != QDataStream::Ok)
        {
#ifdef DEBUG
            qDebug() << "Ошибка записи файла" << stream.status();
#endif
        }
#ifdef DEBUG
        qDebug() << "void ProtectEngine::writeLastTimeRun(): file is opened!)))";
#endif
        m_LicenseFile->close();
    } else {
#ifdef DEBUG
        qDebug() << "void ProtectEngine::writeLastTimeRun(): file is not opened(((";
#endif
    }
}
