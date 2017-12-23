#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    APLLoggingCategoryRegister::instance()->setFilterRulesFromSettings("MainWindowLog");
    w.show();

    return a.exec();
}
