#include <QMessageBox>
#include <QByteArray>
#include "ProtectEngine.h"

#ifdef DEBUG
#include <QDebug>
#endif

ProtectEngine::ProtectEngine(IProtectedProgram *protectedProgram,
                             const QString &licenseFileName)
    : m_ProtectedProgram(protectedProgram)
    , m_LicenseFile(new QFile(licenseFileName))
{
}

ProtectEngine::~ProtectEngine()
{
}

void ProtectEngine::protect()
{
    isValidLicense();
    if(!m_LicenseFile->open(QFile::ReadOnly) && !isValidLicense()) {
        QMessageBox::critical(0, "Start Program Failed", "File " + m_LicenseFile->fileName() + " is corrupt");
        m_LicenseFile->close();
        throw "Renew license!";
    } else {
        m_LicenseFile->close();
        m_ProtectedProgram->startProgram();
    }

    writeLastTimeRun();
}

bool ProtectEngine::isValidLicense()
{
    m_CurrentDate = m_CurrentDate.currentDate();
#ifdef DEBUG
    qDebug() << "bool ProtectEngine::isValidLicense()";
    qDebug() << m_CurrentDate.day();
    qDebug() << m_CurrentDate.month();
    qDebug() << m_CurrentDate.year();
#endif
//    int startTimeDay = m_LicenseFile->read(1).toInt();
//    int startTimeMonth = m_LicenseFile->read(1).toInt();
//    int startTimeYear = m_LicenseFile->read(2).toInt();

//    int endTimeDay = m_LicenseFile->read(1).toInt();
//    int endTimeMonth = m_LicenseFile->read(1).toInt();
//    int endTimeYear = m_LicenseFile->read(2).toInt();

//    int lastTimeDay = m_LicenseFile->read(1).toInt();
//    int lastTimeMonth = m_LicenseFile->read(1).toInt();
//    int lastTimeYear = m_LicenseFile->read(2).toInt();

    QVariant v = m_LicenseFile->read(sizeof(QDate));
    int startTimeDay = v.toDate().day();
    int startTimeMonth = v.toDate().month();
    int startTimeYear = v.toDate().year();

    v = m_LicenseFile->read(sizeof(QDate));
    int endTimeDay = v.toDate().day();
    int endTimeMonth = v.toDate().month();
    int endTimeYear = v.toDate().year();

    v = m_LicenseFile->read(sizeof(QDate));
    int lastTimeDay = v.toDate().day();
    int lastTimeMonth = v.toDate().month();
    int lastTimeYear = v.toDate().year();

#ifdef DEBUG
    qDebug() << "bool ProtectEngine::isValidLicense()";
    qDebug() << startTimeDay;
    qDebug() << startTimeMonth;
    qDebug() << startTimeYear;
#endif
    if(startTimeDay > m_CurrentDate.day()
            && startTimeMonth > m_CurrentDate.month()
            && startTimeYear > m_CurrentDate.year()
            && endTimeDay < m_CurrentDate.day()
            && endTimeMonth < m_CurrentDate.month()
            && endTimeYear < m_CurrentDate.year()
            && lastTimeDay < m_CurrentDate.day()
            && lastTimeMonth < m_CurrentDate.month()
            && lastTimeYear < m_CurrentDate.year()) {
        return true;
    } else {
        return false;
    }
}

void ProtectEngine::writeLastTimeRun()
{
#ifdef DEBUG
        qDebug() << "void ProtectEngine::writeLastTimeRun()";
#endif
    if(m_LicenseFile->open(QFile::WriteOnly)) {
        QVariant v(m_CurrentDate);
         m_LicenseFile->write(v.toString().toStdString().data(), sizeof(QDate));
//        m_LicenseFile->write(reinterpret_cast<const char*>(m_CurrentDate/*.day()*/), sizeof(QDate));
//        m_LicenseFile->write(static_cast<const char*>(m_CurrentDate.month()), 1);
//        m_LicenseFile->write(reinterpret_cast<char*>(m_CurrentDate.year()), 2);
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
