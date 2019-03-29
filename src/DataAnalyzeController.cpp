#include "DataAnalyzeController.h"
#include "mainwindow.h"
#include "APLDB.h"

APL_LOGGING_CATEGORY(DATA_ANALYZE_LOG,        "DataAnalyzeLog")

#define LINE_LIST_INIT  "Normal"<<"Line1"<<"Line2"<<"Line3"<<"Dot1"<<"Dot2"<<"Dot3"<<"Mark1"<<"Mark2"<<"Mark3"
#define COLOR_LIST_INIT "Red"<<"Green"<<"Blue"<<"Purple"<<"Brown"<<"Pink"<<"DeepSkyBlue"<<"Orange"<<"DarkCyan"<<"Gold"

DataAnalyzeController::DataAnalyzeController()
{
    connect(MainWindow::getMainWindow(),  &MainWindow::treeWidgetAddItem, this, &DataAnalyzeController::_setTableList);
    connect(this, &DataAnalyzeController::plotGraph, MainWindow::getMainWindow(),  &MainWindow::plotGraph);
    connect(this, &DataAnalyzeController::clearGraph, MainWindow::getMainWindow(),  &MainWindow::clearGraph);
    connect(this, &DataAnalyzeController::clear_alreadyPloted, MainWindow::getMainWindow(),  &MainWindow::clear_alreadyPloted);

    init();
}

void DataAnalyzeController::init()
{
    MainWindow::getMainWindow()->requestTableList();

    _lineList  << LINE_LIST_INIT;
    _colorList << COLOR_LIST_INIT;
    _available_colorList = _colorList;

    for(int i=0; i<MAX_LINE_NUM; i++){
        _visible[i] = false;
        _scale[i]   = 1.0f;
        _offsetX[i] = 0;
        _offsetY[i] = 0.0f;
        _style[i]   = 0;
        _color[i]   = 0;
    }

    emit tableList1Changed();
    emit tableList2Changed();
    emit tableList3Changed();
    emit tableList4Changed();
    emit tableList5Changed();
    emit tableList6Changed();
    emit tableList7Changed();
    emit tableList8Changed();
    emit tableList9Changed();
    emit tableList10Changed();
}
void DataAnalyzeController::_setTableList(QString table)
{
    MainWindow::getMainWindow()->setComboboxList(table);
    if (!_tableList.contains(table)){
        _tableList<<table;
    }
    _tableList1 = _tableList;
    _tableList2 = _tableList;
    _tableList3 = _tableList;
    _tableList4 = _tableList;
    _tableList5 = _tableList;
    _tableList6 = _tableList;
    _tableList7 = _tableList;
    _tableList8 = _tableList;
    _tableList9 = _tableList;
    _tableList10= _tableList;
}

bool
DataAnalyzeController::_isNumber(QString n)
{
    bool ret = false;
    n.toDouble(&ret);
    return ret;
}

void
DataAnalyzeController::_update_colorList(){
    _available_colorList = _colorList;
    for(int i=0; i<MAX_LINE_NUM; i++){
        if (_visible[i] &&
            _available_colorList.contains(_colorList.at(_color[i])))
        {
            _available_colorList.removeOne(_colorList.at(_color[i]));
        }
    }

    for(int i=0; i<MAX_LINE_NUM; i++){
        if(!_visible[i]){
            switch(i){
            case 0:
                emit colorList1Changed();
                break;
            case 1:
                emit colorList2Changed();
                break;
            case 2:
                emit colorList3Changed();
                break;
            case 3:
                emit colorList4Changed();
                break;
            case 4:
                emit colorList5Changed();
                break;
            case 5:
                emit colorList6Changed();
                break;
            case 6:
                emit colorList7Changed();
                break;
            case 7:
                emit colorList8Changed();
                break;
            case 8:
                emit colorList9Changed();
                break;
            case 9:
                emit colorList10Changed();
                break;
            }
        }
    }
}

void
DataAnalyzeController::_update_hide_tables(QString table)
{
    _tableList1 = _tableList;
    _tableList2 = _tableList;
    _tableList3 = _tableList;
    _tableList4 = _tableList;
    _tableList5 = _tableList;
    _tableList6 = _tableList;
    _tableList7 = _tableList;
    _tableList8 = _tableList;
    _tableList9 = _tableList;
    _tableList10= _tableList;

    for(int i=0; i<MAX_LINE_NUM; i++){
        if(!_visible[i]){
            switch(i){
            case 0:
                _tableList1.swap(0, _tableList1.indexOf(table));
                emit tableList1Changed();
                break;
            case 1:
                _tableList2.swap(0, _tableList2.indexOf(table));
                emit tableList2Changed();
                break;
            case 2:
                _tableList3.swap(0, _tableList3.indexOf(table));
                emit tableList3Changed();
                break;
            case 3:
                _tableList4.swap(0, _tableList4.indexOf(table));
                emit tableList4Changed();
                break;
            case 4:
                _tableList5.swap(0, _tableList5.indexOf(table));
                emit tableList5Changed();
                break;
            case 5:
                _tableList6.swap(0, _tableList6.indexOf(table));
                emit tableList6Changed();
                break;
            case 6:
                _tableList7.swap(0, _tableList7.indexOf(table));
                emit tableList7Changed();
                break;
            case 7:
                _tableList8.swap(0, _tableList8.indexOf(table));
                emit tableList8Changed();
                break;
            case 8:
                _tableList9.swap(0, _tableList9.indexOf(table));
                emit tableList9Changed();
                break;
            case 9:
                _tableList10.swap(0, _tableList10.indexOf(table));
                emit tableList10Changed();
                break;
            }
        }
    }
}

void
DataAnalyzeController::_plot(){
    QCustomPlot* customPlot = MainWindow::getMainWindow()->ui().customPlot;

    emit clearGraph();

    emit clear_alreadyPloted();

    _update_colorList();

    for(int i=0; i<MAX_LINE_NUM; i++){
        if(_visible[i]){
            emit plotGraph(tables[i],
                           fields[i],
                           _offsetX[i],
                           _offsetY[i],
                           _scale[i],
                           _style[i],
                           _color[i],
                           _visible[i],
                           false);
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
    _update_hide_tables(table);
}

void
DataAnalyzeController::setField1(QString field){
    fields[0] = field;
    if(field.compare("TimeUS") != 0) setVisible1(true);
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
DataAnalyzeController::setLineColor1(QString color){
    _color[0] = _colorList.indexOf(color);

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
    _update_hide_tables(table);
}

void
DataAnalyzeController::setField2(QString field){
    fields[1] = field;
    if(field.compare("TimeUS") != 0) setVisible2(true);
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
DataAnalyzeController::setLineColor2(QString color){
    _color[1] = _colorList.indexOf(color);

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
    _update_hide_tables(table);
}

void
DataAnalyzeController::setField3(QString field){
    fields[2] = field;
    if(field.compare("TimeUS") != 0) setVisible3(true);
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
DataAnalyzeController::setLineColor3(QString color){
    _color[2] = _colorList.indexOf(color);

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
    _update_hide_tables(table);
}

void
DataAnalyzeController::setField4(QString field){
    fields[3] = field;
    if(field.compare("TimeUS") != 0) setVisible4(true);
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
DataAnalyzeController::setLineColor4(QString color){
    _color[3] = _colorList.indexOf(color);

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
    _update_hide_tables(table);
}

void
DataAnalyzeController::setField5(QString field){
    fields[4] = field;
    if(field.compare("TimeUS") != 0) setVisible5(true);
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
DataAnalyzeController::setLineColor5(QString color){
    _color[4] = _colorList.indexOf(color);

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
    _update_hide_tables(table);
}

void
DataAnalyzeController::setField6(QString field){
    fields[5] = field;
    if(field.compare("TimeUS") != 0) setVisible6(true);
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
DataAnalyzeController::setLineColor6(QString color){
    _color[5] = _colorList.indexOf(color);

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
    _update_hide_tables(table);
}

void
DataAnalyzeController::setField7(QString field){
    fields[6] = field;
    if(field.compare("TimeUS") != 0) setVisible7(true);
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
DataAnalyzeController::setLineColor7(QString color){
    _color[6] = _colorList.indexOf(color);

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
    _update_hide_tables(table);
}

void
DataAnalyzeController::setField8(QString field){
    fields[7] = field;
    if(field.compare("TimeUS") != 0) setVisible8(true);
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
DataAnalyzeController::setLineColor8(QString color){
    _color[7] = _colorList.indexOf(color);

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
    _update_hide_tables(table);
}

void
DataAnalyzeController::setField9(QString field){
    fields[8] = field;
    if(field.compare("TimeUS") != 0) setVisible9(true);
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
DataAnalyzeController::setLineColor9(QString color){
    _color[8] = _colorList.indexOf(color);

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
    _update_hide_tables(table);
}

void
DataAnalyzeController::setField10(QString field){
    fields[9] = field;
    if(field.compare("TimeUS") != 0) setVisible10(true);
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
DataAnalyzeController::setLineColor10(QString color){
    _color[9] = _colorList.indexOf(color);

    _plot();
}
