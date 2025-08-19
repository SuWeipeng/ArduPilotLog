#include "mainwindow.h"
#include <QApplication>

#define QCDEBUG_FILTER_RULES "MainWindowLog,APLDBLog,APLReadLog,DataAnalyzeLog,DialogLog,APLReadConfLog,APLDataCacheLog"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    APLLoggingCategoryRegister::instance()->setFilterRulesFromSettings(QCDEBUG_FILTER_RULES);
    w.setWindowTitle("ArduPilotLog");
    w.show();

    return a.exec();
}
