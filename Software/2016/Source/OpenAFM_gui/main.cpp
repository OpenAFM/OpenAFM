#include "mainwindow.h"
#include "surfacegraph.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::addLibraryPath("@executable_path/../");
    QApplication a(argc, argv);
    MainWindow w;

    a.setApplicationName("Arduguino 0.1");
    a.setApplicationDisplayName("Arduguino 0.1");
    a.setApplicationVersion("0.1");
    w.show();
    return a.exec();
}
