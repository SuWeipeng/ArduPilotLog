#include "mainwindow.h"
#include "src/APLDB.h"

static void addGraph(QCustomPlot *customPlot, QString &table, QString &field);
static void setData(QCustomPlot *customPlot, QString &table, QString &field);
static void setLegend(QCustomPlot *customPlot);
static void lineModification(QCustomPlot *customPlot);
static void axisModification(QCustomPlot *customPlot, QString &table);

enum PlotOrder{
    ADD_GRAPH,
    SET_DATA,
    SET_LEGEND,
    LINE_MODIFICATION,
    AXIS_MODIFICATION,
    PLOT_NUM
};

static PlotOrder   order = ADD_GRAPH;
static QStringList legends;
static QString     legend;

static void addGraph(QCustomPlot *customPlot, QString &table, QString &field){
    legend = QString("%1.%2").arg(table).arg(field);
    if(!MainWindow::get_customPlot_hold_on()){
        customPlot->clearGraphs();
        legends.clear();
    }
    if(!legends.contains(legend)){
        legends << legend;
        customPlot->addGraph();
        order = SET_DATA;
    }else{
        order = ADD_GRAPH;
    }
}

static void setData(QCustomPlot *customPlot, QString& table, QString& field){
    int length = APLDB::getAPLDB() -> getLen(table, field);
    QVector<double> x(length), y(length);

    if(1){
        APLDB::getAPLDB() -> getData(table, APLDB::getAPLDB()->getItemName(table, 0), length, x);
    }else{
        APLDB::getAPLDB() -> getData(table, APLDB::getAPLDB()->getItemName(table, 1), length, x);
    }
    APLDB::getAPLDB() -> getData(table, field, length, y);

    customPlot->graph()->setData(x, y);

    order = SET_LEGEND;
}

static void setLegend(QCustomPlot *customPlot){
    customPlot->legend->setVisible(true);
    customPlot->legend->setFont(QFont("Helvetica", 9));
    customPlot->legend->setRowSpacing(-3);
    customPlot->graph()->setName(legend);

    order = LINE_MODIFICATION;
}

static void lineModification(QCustomPlot *customPlot){
    QPen pen;
    int  R = 0+qrand()%(255-0);
    int  G = 0+qrand()%(255-0);
    int  B = 0+qrand()%(255-0);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    pen.setColor(QColor(R, G, B));
    customPlot->graph()->setPen(pen);

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
            addGraph(customPlot, table, field);
            break;
        case SET_DATA:
            setData(customPlot, table, field);
            break;
        case SET_LEGEND:
            setLegend(customPlot);
            break;
        case LINE_MODIFICATION:
            lineModification(customPlot);
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
