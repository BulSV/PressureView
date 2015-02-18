#ifndef PROTECTENGINE_H
#define PROTECTENGINE_H

#include <QString>
#include <QFile>
#include <QDate>
#include "IProtectedProgram.h"

class ProtectEngine
{
public:
    ProtectEngine(IProtectedProgram *protectedProgram, const QString &licenseFileName);
    ~ProtectEngine();
    void protect();
private:
    IProtectedProgram *m_ProtectedProgram;
    QFile *m_LicenseFile;
    QDate m_CurrentDate;

    bool isValidLicense();
    void writeLastTimeRun();
};

#endif // PROTECTENGINE_H
