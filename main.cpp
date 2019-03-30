#include "mainwindow.h"
#include <QApplication>

#define QCDEBUG_FILTER_RULES "MainWindowLog,APLDBLog,APLReadLog,DataAnalyzeLog,DialogLog,APLReadConfLog"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    APLLoggingCategoryRegister::instance()->setFilterRulesFromSettings(QCDEBUG_FILTER_RULES);
    w.show();

    return a.exec();
}
