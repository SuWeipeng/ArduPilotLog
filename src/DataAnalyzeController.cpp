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
        }
    }
    customPlot->replot();
}

// Row 1
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

// Row 2
void
DataAnalyzeController::setFieldList2(QString table)
{
    tables[1] = table;

    QList<QTreeWidgetItem*> itemList = MainWindow::getMainWindow()->ui().treeWidget->findItems(table, Qt::MatchCaseSensitive);
    for(int i=0; i<itemList.length(); i++){
        QTreeWidgetItem* item = itemList.at(i);
        _fieldList[1].clear();
        for(int j=0; j<item->childCount(); j++){
            if (!_fieldList[1].contains(table)){
                _fieldList[1]<<item->child(j)->text(0);
            }
        }
        emit fieldList2Changed();
    }
}

void
DataAnalyzeController::setField2(QString field){
    fields[1] = field;
    _plot();
}

void
DataAnalyzeController::setScale2(QString scale){
    if(!_isNumber(scale)) return;

    _scale[1] = scale.left(scale.length()).toFloat();
    _scale[1] = QString::number(_scale[1], 'f', 3).toFloat();
    emit scale2Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetX2(QString offset){
    if(!_isNumber(offset)) return;

    _offsetX[1] = (int)offset.left(offset.length()).toFloat();
    emit offsetX2Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetY2(QString offset){
    if(!_isNumber(offset)) return;

    _offsetY[1] = offset.left(offset.length()).toFloat();
    _offsetY[1] = QString::number(_offsetY[1], 'f', 2).toFloat();
    emit offsetY2Changed();

    _plot();
}

void
DataAnalyzeController::setVisible2(bool visible){
    _visible[1] = visible;
    emit visible2Changed();

    _plot();
}

void
DataAnalyzeController::setLineStyle2(int style){
    _style[1] = style;

    _plot();
}

void
DataAnalyzeController::setLineColor2(int color){
    _color[1] = color;

    _plot();
}

// Row 3
void
DataAnalyzeController::setFieldList3(QString table)
{
    tables[2] = table;

    QList<QTreeWidgetItem*> itemList = MainWindow::getMainWindow()->ui().treeWidget->findItems(table, Qt::MatchCaseSensitive);
    for(int i=0; i<itemList.length(); i++){
        QTreeWidgetItem* item = itemList.at(i);
        _fieldList[2].clear();
        for(int j=0; j<item->childCount(); j++){
            if (!_fieldList[2].contains(table)){
                _fieldList[2]<<item->child(j)->text(0);
            }
        }
        emit fieldList3Changed();
    }
}

void
DataAnalyzeController::setField3(QString field){
    fields[2] = field;
    _plot();
}

void
DataAnalyzeController::setScale3(QString scale){
    if(!_isNumber(scale)) return;

    _scale[2] = scale.left(scale.length()).toFloat();
    _scale[2] = QString::number(_scale[2], 'f', 3).toFloat();
    emit scale3Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetX3(QString offset){
    if(!_isNumber(offset)) return;

    _offsetX[2] = (int)offset.left(offset.length()).toFloat();
    emit offsetX3Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetY3(QString offset){
    if(!_isNumber(offset)) return;

    _offsetY[2] = offset.left(offset.length()).toFloat();
    _offsetY[2] = QString::number(_offsetY[2], 'f', 2).toFloat();
    emit offsetY3Changed();

    _plot();
}

void
DataAnalyzeController::setVisible3(bool visible){
    _visible[2] = visible;
    emit visible3Changed();

    _plot();
}

void
DataAnalyzeController::setLineStyle3(int style){
    _style[2] = style;

    _plot();
}

void
DataAnalyzeController::setLineColor3(int color){
    _color[2] = color;

    _plot();
}

// Row 4
void
DataAnalyzeController::setFieldList4(QString table)
{
    tables[3] = table;

    QList<QTreeWidgetItem*> itemList = MainWindow::getMainWindow()->ui().treeWidget->findItems(table, Qt::MatchCaseSensitive);
    for(int i=0; i<itemList.length(); i++){
        QTreeWidgetItem* item = itemList.at(i);
        _fieldList[3].clear();
        for(int j=0; j<item->childCount(); j++){
            if (!_fieldList[3].contains(table)){
                _fieldList[3]<<item->child(j)->text(0);
            }
        }
        emit fieldList4Changed();
    }
}

void
DataAnalyzeController::setField4(QString field){
    fields[3] = field;
    _plot();
}

void
DataAnalyzeController::setScale4(QString scale){
    if(!_isNumber(scale)) return;

    _scale[3] = scale.left(scale.length()).toFloat();
    _scale[3] = QString::number(_scale[3], 'f', 3).toFloat();
    emit scale4Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetX4(QString offset){
    if(!_isNumber(offset)) return;

    _offsetX[3] = (int)offset.left(offset.length()).toFloat();
    emit offsetX4Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetY4(QString offset){
    if(!_isNumber(offset)) return;

    _offsetY[3] = offset.left(offset.length()).toFloat();
    _offsetY[3] = QString::number(_offsetY[3], 'f', 2).toFloat();
    emit offsetY4Changed();

    _plot();
}

void
DataAnalyzeController::setVisible4(bool visible){
    _visible[3] = visible;
    emit visible4Changed();

    _plot();
}

void
DataAnalyzeController::setLineStyle4(int style){
    _style[3] = style;

    _plot();
}

void
DataAnalyzeController::setLineColor4(int color){
    _color[3] = color;

    _plot();
}

// Row 5
void
DataAnalyzeController::setFieldList5(QString table)
{
    tables[4] = table;

    QList<QTreeWidgetItem*> itemList = MainWindow::getMainWindow()->ui().treeWidget->findItems(table, Qt::MatchCaseSensitive);
    for(int i=0; i<itemList.length(); i++){
        QTreeWidgetItem* item = itemList.at(i);
        _fieldList[4].clear();
        for(int j=0; j<item->childCount(); j++){
            if (!_fieldList[4].contains(table)){
                _fieldList[4]<<item->child(j)->text(0);
            }
        }
        emit fieldList5Changed();
    }
}

void
DataAnalyzeController::setField5(QString field){
    fields[4] = field;
    _plot();
}

void
DataAnalyzeController::setScale5(QString scale){
    if(!_isNumber(scale)) return;

    _scale[4] = scale.left(scale.length()).toFloat();
    _scale[4] = QString::number(_scale[4], 'f', 3).toFloat();
    emit scale5Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetX5(QString offset){
    if(!_isNumber(offset)) return;

    _offsetX[4] = (int)offset.left(offset.length()).toFloat();
    emit offsetX5Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetY5(QString offset){
    if(!_isNumber(offset)) return;

    _offsetY[4] = offset.left(offset.length()).toFloat();
    _offsetY[4] = QString::number(_offsetY[4], 'f', 2).toFloat();
    emit offsetY5Changed();

    _plot();
}

void
DataAnalyzeController::setVisible5(bool visible){
    _visible[4] = visible;
    emit visible5Changed();

    _plot();
}

void
DataAnalyzeController::setLineStyle5(int style){
    _style[4] = style;

    _plot();
}

void
DataAnalyzeController::setLineColor5(int color){
    _color[4] = color;

    _plot();
}

// Row 6
void
DataAnalyzeController::setFieldList6(QString table)
{
    tables[5] = table;

    QList<QTreeWidgetItem*> itemList = MainWindow::getMainWindow()->ui().treeWidget->findItems(table, Qt::MatchCaseSensitive);
    for(int i=0; i<itemList.length(); i++){
        QTreeWidgetItem* item = itemList.at(i);
        _fieldList[5].clear();
        for(int j=0; j<item->childCount(); j++){
            if (!_fieldList[5].contains(table)){
                _fieldList[5]<<item->child(j)->text(0);
            }
        }
        emit fieldList6Changed();
    }
}

void
DataAnalyzeController::setField6(QString field){
    fields[5] = field;
    _plot();
}

void
DataAnalyzeController::setScale6(QString scale){
    if(!_isNumber(scale)) return;

    _scale[5] = scale.left(scale.length()).toFloat();
    _scale[5] = QString::number(_scale[5], 'f', 3).toFloat();
    emit scale6Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetX6(QString offset){
    if(!_isNumber(offset)) return;

    _offsetX[5] = (int)offset.left(offset.length()).toFloat();
    emit offsetX6Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetY6(QString offset){
    if(!_isNumber(offset)) return;

    _offsetY[5] = offset.left(offset.length()).toFloat();
    _offsetY[5] = QString::number(_offsetY[5], 'f', 2).toFloat();
    emit offsetY6Changed();

    _plot();
}

void
DataAnalyzeController::setVisible6(bool visible){
    _visible[5] = visible;
    emit visible6Changed();

    _plot();
}

void
DataAnalyzeController::setLineStyle6(int style){
    _style[5] = style;

    _plot();
}

void
DataAnalyzeController::setLineColor6(int color){
    _color[5] = color;

    _plot();
}

// Row 7
void
DataAnalyzeController::setFieldList7(QString table)
{
    tables[6] = table;

    QList<QTreeWidgetItem*> itemList = MainWindow::getMainWindow()->ui().treeWidget->findItems(table, Qt::MatchCaseSensitive);
    for(int i=0; i<itemList.length(); i++){
        QTreeWidgetItem* item = itemList.at(i);
        _fieldList[6].clear();
        for(int j=0; j<item->childCount(); j++){
            if (!_fieldList[6].contains(table)){
                _fieldList[6]<<item->child(j)->text(0);
            }
        }
        emit fieldList7Changed();
    }
}

void
DataAnalyzeController::setField7(QString field){
    fields[6] = field;
    _plot();
}

void
DataAnalyzeController::setScale7(QString scale){
    if(!_isNumber(scale)) return;

    _scale[6] = scale.left(scale.length()).toFloat();
    _scale[6] = QString::number(_scale[6], 'f', 3).toFloat();
    emit scale7Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetX7(QString offset){
    if(!_isNumber(offset)) return;

    _offsetX[6] = (int)offset.left(offset.length()).toFloat();
    emit offsetX7Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetY7(QString offset){
    if(!_isNumber(offset)) return;

    _offsetY[6] = offset.left(offset.length()).toFloat();
    _offsetY[6] = QString::number(_offsetY[6], 'f', 2).toFloat();
    emit offsetY7Changed();

    _plot();
}

void
DataAnalyzeController::setVisible7(bool visible){
    _visible[6] = visible;
    emit visible7Changed();

    _plot();
}

void
DataAnalyzeController::setLineStyle7(int style){
    _style[6] = style;

    _plot();
}

void
DataAnalyzeController::setLineColor7(int color){
    _color[6] = color;

    _plot();
}

// Row 8
void
DataAnalyzeController::setFieldList8(QString table)
{
    tables[7] = table;

    QList<QTreeWidgetItem*> itemList = MainWindow::getMainWindow()->ui().treeWidget->findItems(table, Qt::MatchCaseSensitive);
    for(int i=0; i<itemList.length(); i++){
        QTreeWidgetItem* item = itemList.at(i);
        _fieldList[7].clear();
        for(int j=0; j<item->childCount(); j++){
            if (!_fieldList[7].contains(table)){
                _fieldList[7]<<item->child(j)->text(0);
            }
        }
        emit fieldList8Changed();
    }
}

void
DataAnalyzeController::setField8(QString field){
    fields[7] = field;
    _plot();
}

void
DataAnalyzeController::setScale8(QString scale){
    if(!_isNumber(scale)) return;

    _scale[7] = scale.left(scale.length()).toFloat();
    _scale[7] = QString::number(_scale[7], 'f', 3).toFloat();
    emit scale8Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetX8(QString offset){
    if(!_isNumber(offset)) return;

    _offsetX[7] = (int)offset.left(offset.length()).toFloat();
    emit offsetX8Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetY8(QString offset){
    if(!_isNumber(offset)) return;

    _offsetY[7] = offset.left(offset.length()).toFloat();
    _offsetY[7] = QString::number(_offsetY[7], 'f', 2).toFloat();
    emit offsetY8Changed();

    _plot();
}

void
DataAnalyzeController::setVisible8(bool visible){
    _visible[7] = visible;
    emit visible8Changed();

    _plot();
}

void
DataAnalyzeController::setLineStyle8(int style){
    _style[7] = style;

    _plot();
}

void
DataAnalyzeController::setLineColor8(int color){
    _color[7] = color;

    _plot();
}

// Row 9
void
DataAnalyzeController::setFieldList9(QString table)
{
    tables[8] = table;

    QList<QTreeWidgetItem*> itemList = MainWindow::getMainWindow()->ui().treeWidget->findItems(table, Qt::MatchCaseSensitive);
    for(int i=0; i<itemList.length(); i++){
        QTreeWidgetItem* item = itemList.at(i);
        _fieldList[8].clear();
        for(int j=0; j<item->childCount(); j++){
            if (!_fieldList[8].contains(table)){
                _fieldList[8]<<item->child(j)->text(0);
            }
        }
        emit fieldList9Changed();
    }
}

void
DataAnalyzeController::setField9(QString field){
    fields[8] = field;
    _plot();
}

void
DataAnalyzeController::setScale9(QString scale){
    if(!_isNumber(scale)) return;

    _scale[8] = scale.left(scale.length()).toFloat();
    _scale[8] = QString::number(_scale[8], 'f', 3).toFloat();
    emit scale9Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetX9(QString offset){
    if(!_isNumber(offset)) return;

    _offsetX[8] = (int)offset.left(offset.length()).toFloat();
    emit offsetX9Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetY9(QString offset){
    if(!_isNumber(offset)) return;

    _offsetY[8] = offset.left(offset.length()).toFloat();
    _offsetY[8] = QString::number(_offsetY[8], 'f', 2).toFloat();
    emit offsetY9Changed();

    _plot();
}

void
DataAnalyzeController::setVisible9(bool visible){
    _visible[8] = visible;
    emit visible9Changed();

    _plot();
}

void
DataAnalyzeController::setLineStyle9(int style){
    _style[8] = style;

    _plot();
}

void
DataAnalyzeController::setLineColor9(int color){
    _color[8] = color;

    _plot();
}

// Row 10
void
DataAnalyzeController::setFieldList10(QString table)
{
    tables[9] = table;

    QList<QTreeWidgetItem*> itemList = MainWindow::getMainWindow()->ui().treeWidget->findItems(table, Qt::MatchCaseSensitive);
    for(int i=0; i<itemList.length(); i++){
        QTreeWidgetItem* item = itemList.at(i);
        _fieldList[9].clear();
        for(int j=0; j<item->childCount(); j++){
            if (!_fieldList[9].contains(table)){
                _fieldList[9]<<item->child(j)->text(0);
            }
        }
        emit fieldList10Changed();
    }
}

void
DataAnalyzeController::setField10(QString field){
    fields[9] = field;
    _plot();
}

void
DataAnalyzeController::setScale10(QString scale){
    if(!_isNumber(scale)) return;

    _scale[9] = scale.left(scale.length()).toFloat();
    _scale[9] = QString::number(_scale[9], 'f', 3).toFloat();
    emit scale10Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetX10(QString offset){
    if(!_isNumber(offset)) return;

    _offsetX[9] = (int)offset.left(offset.length()).toFloat();
    emit offsetX10Changed();

    _plot();
}

void
DataAnalyzeController::setOffsetY10(QString offset){
    if(!_isNumber(offset)) return;

    _offsetY[9] = offset.left(offset.length()).toFloat();
    _offsetY[9] = QString::number(_offsetY[9], 'f', 2).toFloat();
    emit offsetY10Changed();

    _plot();
}

void
DataAnalyzeController::setVisible10(bool visible){
    _visible[9] = visible;
    emit visible10Changed();

    _plot();
}

void
DataAnalyzeController::setLineStyle10(int style){
    _style[9] = style;

    _plot();
}

void
DataAnalyzeController::setLineColor10(int color){
    _color[9] = color;

    _plot();
}
