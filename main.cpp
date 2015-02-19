#ifdef DEBUG
#include <QDebug>
#endif

#include <QApplication>
#include "Dialog.h"
#include "ProtectEngine.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

#if defined (Q_OS_UNIX)
    app.setWindowIcon(QIcon(":/Resources/PressureView.png"));
#endif
    Dialog *dialog = new Dialog();
    dialog->setWindowTitle(QString::fromUtf8("Pressure View"));
    ProtectEngine *pe = new ProtectEngine(dialog, "status.sys");
    pe->protect();

    return app.exec();
}

