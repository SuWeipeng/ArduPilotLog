#include "DataAnalyzeController.h"
#include "mainwindow.h"
#include "APLDB.h"

APL_LOGGING_CATEGORY(DATA_ANALYZE_LOG,        "DataAnalyzeLog")

#define LINE_LIST_INIT  "Normal"<<"Line1"<<"Line2"<<"Line3"<<"Dot1"<<"Dot2"<<"Dot3"<<"Mark1"<<"Mark2"<<"Mark3"
#define COLOR_LIST_INIT "Red"<<"Green"<<"Blue"<<"Purple"<<"Brown"<<"Pink"<<"DeepSkyBlue"<<"Orange"<<"DarkCyan"<<"Gold"

DataAnalyzeController::DataAnalyzeController()
{
    connect(MainWindow::getMainWindow(),  &MainWindow::treeWidgetAddItem, this, &DataAnalyzeController::_setTableList);

    MainWindow::getMainWindow()->requestTableList();

    _lineList  << LINE_LIST_INIT;
    _colorList << COLOR_LIST_INIT;

    for(int i=0; i<MAX_LINE_NUM; i++){
        _visible[i] = false;
        _scale[i]   = 1.0f;
        _offsetX[i] = 0;
        _offsetY[i] = 0.0f;
        _style[i]   = 0;
        _color[i]   = 0;

        shapes[i] << QCPScatterStyle::ssCircle;
        shapes[i] << QCPScatterStyle::ssDisc;
        shapes[i] << QCPScatterStyle::ssDiamond;
        shapes[i] << QCPScatterStyle::ssCrossCircle;
        shapes[i] << QCPScatterStyle::ssPlusCircle;

        colors[i] << QColor(255, 0, 0);    // Red
        colors[i] << QColor(34, 139, 34);  // Green
        colors[i] << QColor(0, 0, 255);     // Blue
        colors[i] << QColor(160, 32, 240);  // Purple
        colors[i] << QColor(139, 35, 35);   // Brown
        colors[i] << QColor(255, 20, 147);  // Pink
        colors[i] << QColor(0, 104, 139);   // DeepSkyBlue
        colors[i] << QColor(255, 140, 0);   // Orange
        colors[i] << QColor(0, 139, 139);   // DarkCyan
        colors[i] << QColor(205, 149, 12);  // Gold
    }
}

void DataAnalyzeController::_setTableList(QString table)
{
    if (!_tableList.contains(table)){
        _tableList<<table;
        emit tableListChanged();
    }
}

void
DataAnalyzeController::setFieldList1(QString table)
{
    tables[0] = table;

    QList<QTreeWidgetItem*> itemList = MainWindow::getMainWindow()->ui().treeWidget->findItems(table, Qt::MatchCaseSensitive);
    for(int i=0; i<itemList.length(); i++){
        QTreeWidgetItem* item = itemList.at(i);
        _fieldList[0].clear();
        for(int j=0; j<item->childCount(); j++){
            if (!_fieldList[0].contains(table)){
                _fieldList[0]<<item->child(j)->text(0);
            }
        }
        emit fieldList1Changed();
    }
}

void
DataAnalyzeController::setField1(QString field){
    fields[0] = field;
    _plot();
}

bool
DataAnalyzeController::_isNumber(QString n)
{
    bool ret = false;
    n.toDouble(&ret);
    return ret;
}

void
DataAnalyzeController::_lineStyle(int index, int i){
    QCustomPlot* customPlot = MainWindow::getMainWindow()->ui().customPlot;
    QPen pen;
    switch (index) {
    case 0: // Normal
        pen.setColor(colors[0].at(_color[i]));
        customPlot->graph()->setPen(pen);
        break;
    case 1: // Line1
        pen.setColor(colors[0].at(_color[i]));
        pen.setWidthF(2);
        customPlot->graph()->setPen(pen);
        break;
    case 2: // Line2
        pen.setColor(colors[0].at(_color[i]));
        customPlot->graph()->setPen(pen);
        customPlot->graph()->setBrush(QBrush(QColor(0, 0, 255, 20)));
        break;
    case 3: // Line3
        pen.setColor(colors[0].at(_color[i]));
        pen.setWidthF(2);
        customPlot->graph()->setPen(pen);
        customPlot->graph()->setBrush(QBrush(QColor(0, 0, 255, 20)));
        break;
    case 4: // Dot1
        pen.setColor(colors[0].at(_color[i]));
        pen.setStyle(Qt::DashLine);
        customPlot->graph()->setPen(pen);
        break;
    case 5: // Dot2
        pen.setColor(colors[0].at(_color[i]));
        pen.setStyle(Qt::DotLine);
        pen.setWidthF(2);
        customPlot->graph()->setPen(pen);
        break;
    case 6: // Dot3
        pen.setColor(colors[0].at(_color[i]));
        pen.setStyle(Qt::DashDotLine);
        customPlot->graph()->setPen(pen);
        customPlot->graph()->setBrush(QBrush(QColor(0, 255, 0, 20)));
        break;
    case 7: // Mark1
        pen.setColor(colors[0].at(_color[i]));
        pen.setWidthF(1);
        customPlot->graph()->setPen(pen);
        customPlot->graph()->setScatterStyle(QCPScatterStyle(shapes[0].at(0)));
        break;
    case 8: // Mark2
        pen.setColor(colors[0].at(_color[i]));
        pen.setWidthF(1);
        customPlot->graph()->setPen(pen);
        customPlot->graph()->setScatterStyle(QCPScatterStyle(shapes[0].at(1)));
        break;
    case 9: // Mark3
        pen.setColor(colors[0].at(_color[i]));
        customPlot->graph()->setPen(pen);
        customPlot->graph()->setLineStyle(QCPGraph::lsNone);
        customPlot->graph()->setScatterStyle(QCPScatterStyle(shapes[0].at(0)));
        break;
    default:
        break;
    }
}

void
DataAnalyzeController::_plot(){
    QCustomPlot* customPlot = MainWindow::getMainWindow()->ui().customPlot;

    customPlot->legend->clear();
    customPlot->legend->setVisible(false);
    customPlot->clearGraphs();
    customPlot->replot();

    for(int i=0; i<MAX_LINE_NUM; i++){
        if(_visible[i]){
            customPlot->addGraph();
            switch (i) {
            case 0:{
                int length = APLDB::getAPLDB() -> getLen(tables[i], fields[i]);
                QVector<double> x(length), y(length);

                APLDB::getAPLDB() -> getData(tables[i], APLDB::getAPLDB()->getItemName(tables[i], MainWindow::get_comboBoxIndex()), length, x, _offsetX[i]);
                APLDB::getAPLDB() -> getData(tables[i], fields[i], length, y, _offsetY[i], _scale[i]);
                customPlot->graph()->setData(x, y);

                customPlot->legend->setVisible(true);
                customPlot->legend->setFont(QFont("Helvetica", 9));
                customPlot->legend->setRowSpacing(-3);
                customPlot->graph()->setName(QString("%1.%2").arg(tables[i]).arg(fields[i]));

                _lineStyle(_style[i], i);

                customPlot->xAxis->setLabel(APLDB::getAPLDB()->getItemName(tables[i], MainWindow::get_comboBoxIndex()));
                customPlot->yAxis->setLabel("y");
                customPlot->rescaleAxes();

                customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
                break;
            }
            default:
                break;
            }

        }
    }
    customPlot->replot();
}

void
DataAnalyzeController::setScale1(QString scale){
    if(!_isNumber(scale)) return;

    _scale[0] = scale.left(scale.length()).toFloat();
    _scale[0] = QString::number(_scale[0], 'f', 3).toFloat();
    emit scale1Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetX1(QString offset){
    if(!_isNumber(offset)) return;

    _offsetX[0] = (int)offset.left(offset.length()).toFloat();
    emit offsetX1Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetY1(QString offset){
    if(!_isNumber(offset)) return;

    _offsetY[0] = offset.left(offset.length()).toFloat();
    _offsetY[0] = QString::number(_offsetY[0], 'f', 2).toFloat();
    emit offsetY1Changed();

    _plot();
}

void
DataAnalyzeController::setVisible1(bool visible){
    _visible[0] = visible;
    emit visible1Changed();

    _plot();
}

void
DataAnalyzeController::setLineStyle1(int style){
    _style[0] = style;

    _plot();
}

void
DataAnalyzeController::setLineColor1(int color){
    _color[0] = color;

    _plot();
}
