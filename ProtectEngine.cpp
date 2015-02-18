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
        QMessageBox::critical(0, "Start Program Failed", "File " + m_LicenseFile->fileName() + " is corrupt");        
        m_LicenseFile->close();
//        writeLastTimeRun();
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
        QDate date;

        stream >> date;
#ifdef DEBUG
        if(stream.status() != QDataStream::Ok)
        {
            qDebug() << "Ошибка чтения файла" << stream.status();
        }
#endif
        int startTimeDay = date.day();
        int startTimeMonth = date.month();
        int startTimeYear = date.year();

        stream >> date;
#ifdef DEBUG
        if(stream.status() != QDataStream::Ok)
        {
            qDebug() << "Ошибка чтения файла" << stream.status();
        }
#endif
        int endTimeDay = date.day();
        int endTimeMonth = date.month();
        int endTimeYear = date.year();

        stream >> date;
#ifdef DEBUG
        if(stream.status() != QDataStream::Ok)
        {
            qDebug() << "Ошибка чтения файла" << stream.status();
        }
#endif
        int lastTimeDay = date.day();
        int lastTimeMonth = date.month();
        int lastTimeYear = date.year();
#ifdef DEBUG
        qDebug() << "READ: bool ProtectEngine::isValidLicense()";
        qDebug() << "Start Time:";
        qDebug() << startTimeDay;
        qDebug() << startTimeMonth;
        qDebug() << startTimeYear;
        qDebug() << "End Time:";
        qDebug() << endTimeDay;
        qDebug() << endTimeMonth;
        qDebug() << endTimeYear;
        qDebug() << "Last Time:";
        qDebug() << lastTimeDay;
        qDebug() << lastTimeMonth;
        qDebug() << lastTimeYear;
#endif
        if(startTimeDay <= m_CurrentDate->day()
                && startTimeMonth <= m_CurrentDate->month()
                && startTimeYear <= m_CurrentDate->year()
                && endTimeDay >= m_CurrentDate->day()
                && endTimeMonth >= m_CurrentDate->month()
                && endTimeYear >= m_CurrentDate->year()
                && lastTimeDay <= m_CurrentDate->day()
                && lastTimeMonth <= m_CurrentDate->month()
                && lastTimeYear <= m_CurrentDate->year()) {
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
    if(m_LicenseFile->open(QFile::WriteOnly | QFile::Truncate)) {
        QDataStream stream(m_LicenseFile);
        stream.setVersion(QDataStream::Qt_5_2);
        QDate date(2015, 2, 17);
        stream << date;
        date.setDate(2015, 2, 19);
        stream << date;
        stream << *m_CurrentDate;
        if(stream.status() != QDataStream::Ok)
        {
            qDebug() << "Ошибка записи файла" << stream.status();
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
