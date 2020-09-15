#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.show();

    a.setQuitOnLastWindowClosed(false);  // программа не остановится, а только закроется окно


    return a.exec();
}


