#ifndef IPROTECTEDPROGRAM_H
#define IPROTECTEDPROGRAM_H

class IProtectedProgram
{
public:
    virtual ~IProtectedProgram() {}
    virtual void startProgram() = 0;
};

#endif // IPROTECTEDPROGRAM_H
