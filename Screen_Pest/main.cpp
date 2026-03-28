#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto w = FloatingWindowFactory();
    if(w)
        w->show();
    return a.exec();
}
