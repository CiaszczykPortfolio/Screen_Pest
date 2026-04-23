#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QString rootDir = QCoreApplication::applicationDirPath() + "/../../objects";
    MainWindow window(rootDir);
    window.show();

    return a.exec();
}
