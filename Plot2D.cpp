#include "mainwindow.h"
#include "src/APLDB.h"

void MainWindow::_addNewData(QCustomPlot *customPlot, QString& table, QString& field)
{
    int length = 0;
    length  = APLDB::getAPLDB() -> getLen(table, field);
//    qCDebug(MAIN_WINDOW_LOG)<<table<<field<<length;

    QVector<double> x(length), y(length); // initialize with entries 0..100

    for (int i=0; i<length; i++)
    {
        if(1){
            APLDB::getAPLDB() -> getData(table, APLDB::getAPLDB()->getItemName(table, 0), length, x);
        }else{
            APLDB::getAPLDB() -> getData(table, APLDB::getAPLDB()->getItemName(table, 1), length, x);
        }

        APLDB::getAPLDB() -> getData(table, field, length, y);
    }
    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    if(1){
        customPlot->xAxis->setLabel(APLDB::getAPLDB()->getItemName(table, 0));
    }else{
        customPlot->xAxis->setLabel(APLDB::getAPLDB()->getItemName(table, 1));
    }
    customPlot->yAxis->setLabel("y");

    customPlot->rescaleAxes();

    customPlot->replot();
}
