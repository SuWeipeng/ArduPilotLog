#include "mainwindow.h"
#include "src/APLDB.h"

static void addGraph(QCustomPlot *customPlot);
static void setData(QCustomPlot *customPlot, QString &table, QString &field, int index = 0);
static void setLegend(QCustomPlot *customPlot);
static void axisModification(QCustomPlot *customPlot, QString &table);

enum PlotOrder{
    ADD_GRAPH,
    SET_DATA,
    SET_LEGEND,
    AXIS_MODIFICATION,
    PLOT_NUM
};

static PlotOrder order = ADD_GRAPH;

static void addGraph(QCustomPlot *customPlot){
    customPlot->addGraph();

    order = SET_DATA;
}

static void setData(QCustomPlot *customPlot, QString& table, QString& field, int index){
    int length = APLDB::getAPLDB() -> getLen(table, field);
    QVector<double> x(length), y(length);

    if(1){
        APLDB::getAPLDB() -> getData(table, APLDB::getAPLDB()->getItemName(table, 0), length, x);
    }else{
        APLDB::getAPLDB() -> getData(table, APLDB::getAPLDB()->getItemName(table, 1), length, x);
    }
    APLDB::getAPLDB() -> getData(table, field, length, y);
    customPlot->graph(index)->setData(x, y);

    order = SET_LEGEND;
}

static void setLegend(QCustomPlot *customPlot){

    order = AXIS_MODIFICATION;
}

static void axisModification(QCustomPlot *customPlot, QString &table){
    if(1){
        customPlot->xAxis->setLabel(APLDB::getAPLDB()->getItemName(table, 0));
    }else{
        customPlot->xAxis->setLabel(APLDB::getAPLDB()->getItemName(table, 1));
    }
    customPlot->yAxis->setLabel("y");

    customPlot->rescaleAxes();

    order = ADD_GRAPH;
}

void MainWindow::_plot2d(QCustomPlot *customPlot, QString& table, QString& field)
{
    for(int i=0; i<PLOT_NUM; i++){
        switch(order){
        case ADD_GRAPH:
            addGraph(customPlot);
            break;
        case SET_DATA:
            setData(customPlot, table, field);
            break;
        case SET_LEGEND:
            setLegend(customPlot);
            break;
        case AXIS_MODIFICATION:
            axisModification(customPlot, table);
            break;
        default:
            break;
        }
    }

    customPlot->replot();
}
