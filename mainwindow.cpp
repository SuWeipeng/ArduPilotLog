﻿#include <QDebug>
#include <QtQml>
#include <QTreeWidgetItem>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "src/APLRead.h"
#include "src/APLReadConf.h"
#include "src/APLDB.h"
#include "src/DataAnalyze.h"
#include "src/DataAnalyzeController.h"

APL_LOGGING_CATEGORY(MAIN_WINDOW_LOG,        "MainWindowLog")

#define ARRAY_SIZE(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

bool        MainWindow::_customPlot_hold_on;
int         MainWindow::_comboBoxIndex;
bool        MainWindow::_X_axis_changed;
MainWindow* MainWindow::_instance;

static const char *rgDockWidgetNames[] = {
    "DATA Analyze"
};

enum DockWidgetTypes {
    DATA_ANALYZE
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _dialog(new Dialog)
    , _dialog_load(new DialogLoad)
    , _table("")
    , _field("")
    , _comboBoxListINIT(true)
    , _action_bold(0x1<<0)
    , _conf_plot(false)
    , _is_constant(false)
    , _replot(false)
    , _plotConf(false)
{
    qmlRegisterType<DataAnalyzeController>("ArduPilotLog.Controllers", 1, 0, "DataAnalyzeController");

    int screenWidth=QApplication::desktop()->width();
    int screenHeight=QApplication::desktop()->height();
    _ui.setupUi(this);
    this->resize(screenWidth/2, screenHeight/2);
    _buildCommonWidgets();
    _ui.splitter->setStretchFactor(0, 1);
    _ui.splitter->setStretchFactor(1, 8);
    initTreeWidget();
    _instance = this;

    for(int i=0; i<10; i++){
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

    connect(_ui.actionOpenArduPilotLog,  &QAction::triggered, _ui.treeWidget, &QTreeWidget::clear);
    connect(_ui.actionOpenArduPilotLog,  &QAction::triggered, this, &MainWindow::clearGraph);
    connect(_ui.actionOpenArduPilotLog,  &QAction::triggered, _dialog, &Dialog::showFile);
    connect(_ui.actionLoad,  &QAction::triggered, _dialog_load, &DialogLoad::showFile);
    connect(_ui.actionSaveDBFile,  &QAction::triggered, _dialog, &Dialog::saveFile);
    connect(_dialog->getAPLRead(),  &APLRead::fileOpened, this, &MainWindow::_fileOpenedTrigger);
    connect(_dialog_load->getAPLReadConf(),  &APLReadConf::fileOpened, this, &MainWindow::_confOpenedTrigger);
    connect(_dialog,  &Dialog::saveSuccess, this, &MainWindow::_saveSuccessMessage);
    connect(this,  &MainWindow::treeWidgetAddItem, this, &MainWindow::setComboboxList);
}

MainWindow::~MainWindow()
{
    delete _dialog;
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    for (int i = 0, end = ARRAY_SIZE(rgDockWidgetNames); i < end; i++) {
        const char* pDockWidgetName = rgDockWidgetNames[i];
        if(_mapName2DockWidget[pDockWidgetName]){
            if(_mapName2DockWidget[pDockWidgetName]->isVisible()){
                _mapName2DockWidget[pDockWidgetName]->closeEvent(event);
            }
        }
    }
    QWidget::closeEvent(event);
}

void MainWindow::_buildCommonWidgets(void)
{
    // Populate widget menu
    for (int i = 0, end = ARRAY_SIZE(rgDockWidgetNames); i < end; i++) {

        const char* pDockWidgetName = rgDockWidgetNames[i];

        // Add to menu
        QAction* action = new QAction(pDockWidgetName, this);
        action->setCheckable(true);
        action->setData(i);
        connect(action, &QAction::triggered, this, &MainWindow::_showDockWidgetAction);
        _ui.menuTools->addAction(action);
        _mapName2Action[pDockWidgetName] = action;
    }
}

void MainWindow::_showDockWidgetAction(bool show)
{
    QAction* action = qobject_cast<QAction*>(QObject::sender());
    Q_ASSERT(action);
    _showDockWidget(rgDockWidgetNames[action->data().toInt()], show);
}

/// Shows or hides the specified dock widget, creating if necessary
void MainWindow::_showDockWidget(const QString& name, bool show)
{
    // Create the inner widget if we need to
    if (!_mapName2DockWidget.contains(name)) {
        if(!_createInnerDockWidget(name)) {
            qWarning() << "Trying to load non existent widget:" << name;
            return;
        }
    }else{
        if(name.compare(rgDockWidgetNames[0]) == 0){
            requestTableList();
        }
    }
    Q_ASSERT(_mapName2DockWidget.contains(name));
    APLDockWidget* dockWidget = _mapName2DockWidget[name];
    Q_ASSERT(dockWidget);
    dockWidget->setVisible(show);
    Q_ASSERT(_mapName2Action.contains(name));
    _mapName2Action[name]->setChecked(show);
}

/// Creates the specified inner dock widget and adds to the QDockWidget
bool MainWindow::_createInnerDockWidget(const QString& widgetName)
{
    APLDockWidget* widget = NULL;
    QAction *action = _mapName2Action[widgetName];
    if(action) {
        switch(action->data().toInt()) {
            case DATA_ANALYZE:
                widget = new DataAnalyze(widgetName, action, this);
                break;
        }
        if(widget) {
            _mapName2DockWidget[widgetName] = widget;
        }
    }
    return widget != NULL;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);

   QWidget * mainWidget=_ui.mainWidget;
   QRect resizeRect=mainWidget->rect();
   static float baseWidth=400;
   static float baseHeight=300;
   static float widgetWidth=resizeRect.width();
   static float widgetHeight=resizeRect.height();

   float horRatio=this->rect().width()/baseWidth;
   float verRatio=this->rect().height()/baseHeight;

   //resize the verticalLayoutWidget
   resizeRect.setWidth(widgetWidth*horRatio);
   resizeRect.setHeight(widgetHeight*verRatio - 10);

   //set Geometry
   mainWidget->setGeometry(resizeRect);
}

void MainWindow::_fileOpenedTrigger()
{
    QTreeWidgetItem* groupItem;
    int GroupCount     = APLDB::getAPLDB() -> getGroupCount();
    int ItemCount      = 0;
    int treeGroupCount = 0;

    _groupName.clear();

    for(int i = 1; i <= GroupCount; i++){
        if(APLDB::getAPLDB() -> isEmpty(APLDB::getAPLDB() -> getGroupName(i)) == false){
            _groupName << QString("%1").arg(APLDB::getAPLDB() -> getGroupName(i));
            treeGroupCount++;
        }
    }

    _groupName.sort();

    for(int i = 0; i < treeGroupCount; i++){
        QString table_name = _groupName.at(i);
        groupItem = new QTreeWidgetItem(_ui.treeWidget,QStringList(table_name));
        ItemCount = APLDB::getAPLDB() -> getItemCount(table_name);
        for (int j = 1; j <= ItemCount; j++)
        {
            QTreeWidgetItem *item=new QTreeWidgetItem(groupItem,QStringList(APLDB::getAPLDB() -> getItemName(table_name, j)));
            item->setCheckState(0, Qt::Unchecked);
            groupItem->addChild(item);
        }
    }

    requestTableList();
}

void MainWindow::requestTableList()
{
    for(int i=0; i<_ui.treeWidget->topLevelItemCount(); i++){
        emit treeWidgetAddItem(_ui.treeWidget->topLevelItem(i)->text(0));
    }
}

void MainWindow::setComboboxList(QString table)
{
    QString            Item0;
    QString            Item1;

    Item0 = APLDB::getAPLDB()->getItemName(table, 0);
    Item1 = APLDB::getAPLDB()->getItemName(table, 1);
    if(!_comboBoxList.contains(Item0)){
        _comboBoxList<<Item0;
        _ui.comboBox->addItem(Item0);
    }
    if(!_comboBoxList.contains(Item1)){
        _comboBoxList<<Item1;
        _ui.comboBox->addItem(Item1);
    }
    if(_comboBoxListINIT){
        _comboBoxListINIT = false;
        _ui.comboBox->setCurrentIndex(1);
    }
}

void MainWindow::_plotGraph(QTreeWidgetItem *item, int column)
{
    QTreeWidgetItem*   parent = item->parent();
    QCustomPlot*       customPlot = MainWindow::getMainWindow()->ui().customPlot;
    int                index;    

    if(NULL==parent) return;

    index = parent->indexOfChild(item);
    _table = parent->text(column);
    _field = parent->child(index)->text(column);

    plotGraph(_table,
              _field,
              0,
              0,
              1,
              0,
              0,
              0,
              true);

    customPlot->replot();
}

void MainWindow::_removeGraph(QTreeWidgetItem *item, int column)
{
    QTreeWidgetItem*   parent = item->parent();
    QCustomPlot*       customPlot = MainWindow::getMainWindow()->ui().customPlot;
    int                index;

    if(NULL==parent) return;

    index = parent->indexOfChild(item);
    _table = parent->text(column);
    _field = parent->child(index)->text(column);

    QString remove_target = QString("%1.%2").arg(_table).arg(_field);

    QStringList alreadyPloted;
    if(_alreadyPloted.contains(remove_target)){
        _alreadyPloted.removeOne(remove_target);
        alreadyPloted = _alreadyPloted;
        clear_alreadyPloted();

        if(alreadyPloted.length() == 0){
            _ui.customPlot->legend->setVisible(false);
        }
    } else {
        return;
    }

    _ui.customPlot->clearGraphs();

    for(int i=0; i<alreadyPloted.length(); i++){
        QStringList list = alreadyPloted.at(i).split(".");
        _table = list[0];
        _field = list[1];
        plotGraph(_table,
                  _field,
                  0,
                  0,
                  1,
                  0,
                  0,
                  0,
                  true);
    }

    customPlot->replot();
}

void MainWindow::clearGraph()
{
    clear_alreadyPloted();
    _ui.customPlot->legend->setVisible(false);
    _ui.customPlot->clearGraphs();
    _ui.customPlot->replot();
    _clearTreeWidget(_ui.treeWidget);
}

void MainWindow::clearGraphNotTree()
{
    clear_alreadyPloted();
    _ui.customPlot->legend->setVisible(false);
    _ui.customPlot->clearGraphs();
    _ui.customPlot->replot();
//    _clearTreeWidget(_ui.treeWidget);
}

void MainWindow::_resetGraph()
{
    _action_bold = (0x1<<0);
    _ui.customPlot->axisRect()->setRangeZoomAxes(_ui.customPlot->xAxis, _ui.customPlot->yAxis);
    _ui.customPlot->rescaleAxes();
    _ui.customPlot->replot();
}

void MainWindow::_zoomX()
{
    _action_bold = (0x1<<1);
    _ui.customPlot->axisRect()->setRangeZoomAxes(_ui.customPlot->xAxis, NULL);
}

void MainWindow::_zoomY()
{
    _action_bold = (0x1<<2);
    _ui.customPlot->axisRect()->setRangeZoomAxes(NULL, _ui.customPlot->yAxis);
}

void MainWindow::_zoomAll()
{
    _action_bold = (0x1<<3);
    _ui.customPlot->axisRect()->setRangeZoomAxes(_ui.customPlot->xAxis, _ui.customPlot->yAxis);
}

void MainWindow::on_customPlot_customContextMenuRequested()
{
    QFont ft;
    QMenu *menu=new QMenu(_ui.customPlot);

    // Clear graph
    QAction* pClearGraph = new QAction(tr("Clear"), this);
    connect(pClearGraph, &QAction::triggered, this, &MainWindow::clearGraph);
    menu->addAction(pClearGraph);
    // Reset graph
    QAction* pResetGraph = new QAction(tr("Reset graph"), this);
    ft.setBold((_action_bold & 0x1) != 0);
    pResetGraph->setFont(ft);
    connect(pResetGraph, &QAction::triggered, this, &MainWindow::_resetGraph);
    menu->addAction(pResetGraph);
    // Zoom X
    QAction* pZoomX = new QAction(tr("zoom X"), this);
    ft.setBold((_action_bold & 0x2) != 0);
    pZoomX->setFont(ft);
    connect(pZoomX, &QAction::triggered, this, &MainWindow::_zoomX);
    menu->addAction(pZoomX);
    // Zoom Y
    QAction* pZoomY = new QAction(tr("zoom Y"), this);
    ft.setBold((_action_bold & 0x4) != 0);
    pZoomY->setFont(ft);
    connect(pZoomY, &QAction::triggered, this, &MainWindow::_zoomY);
    menu->addAction(pZoomY);
    // Zoom All
    QAction* pZoomAll = new QAction(tr("Zoom All"), this);
    ft.setBold((_action_bold & 0x8) != 0);
    pZoomAll->setFont(ft);
    connect(pZoomAll, &QAction::triggered, this, &MainWindow::_zoomAll);
    menu->addAction(pZoomAll);
    menu->exec(QCursor::pos());
}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    if(arg1.compare("") == 0) return;
    _X_axis_changed = true;
    _comboBoxIndex  = _ui.comboBox->currentIndex();
    _ui.customPlot->legend->setVisible(false);
    _ui.customPlot->clearGraphs();
    _ui.customPlot->xAxis->setLabel(MainWindow::getMainWindow()->ui().comboBox->currentText());
    _ui.customPlot->replot();

    QStringList alreadyPloted = _alreadyPloted;
    clear_alreadyPloted();

    if(!_replot) return;

    for(int i=0; i<alreadyPloted.length(); i++){
        QStringList list = alreadyPloted.at(i).split(".");
        _table = list[0];
        _field = list[1];
        plotGraph(_table,
                  _field,
                  0,
                  0,
                  1,
                  0,
                  0,
                  0,
                  true);
    }

    _ui.customPlot->replot();
}

void MainWindow::_saveSuccessMessage(){
    QMessageBox::information(this,tr("Information"),tr("Save success"));
}

void MainWindow::initTreeWidget(){
    _ui.treeWidget->setColumnCount(1);
    _ui.treeWidget->setHeaderLabel(tr("ArduPilot Log"));
    _ui.treeWidget->setHeaderHidden(true);

    connect(_ui.treeWidget, &QTreeWidget::itemChanged, this, &MainWindow::itemChangedSlot);
}

bool MainWindow::isTopItem(QTreeWidgetItem *item)
{
    if(!item) return false;
    if(!item->parent()) return true;
    return false;
}

void MainWindow::setChildCheckState(QTreeWidgetItem *item, Qt::CheckState cs, int column)
{
    if(!item) return;
    for (int i=0;i<item->childCount();i++)
    {
        QTreeWidgetItem* child=item->child(i);
        if(child->checkState(0)!=cs)
        {
            child->setCheckState(0, cs);
        }
    }
    setParentCheckState(item->parent(), column);
}

void MainWindow::setParentCheckState(QTreeWidgetItem *item, int column)
{
    if(!item) return;
    int selectedCount=0;
    int childCount = item->childCount();

    if(_plotConf){
        clear_alreadyPloted();
        _ui.customPlot->legend->setVisible(false);
        _ui.customPlot->clearGraphs();
        _ui.customPlot->replot();
        _plotConf = false;
    }

    for (int i=0;i<childCount;i++)
    {
        QTreeWidgetItem* child= item->child(i);
        if(child->checkState(column)==Qt::Checked)
        {
            selectedCount++;
            _plotGraph(item->child(i), column);
        } else {
            _removeGraph(item->child(i), column);
        }
    }

    if(selectedCount == 0) {
        item->setCheckState(column,Qt::Unchecked);
    } else if (selectedCount == childCount) {
        item->setCheckState(column,Qt::Checked);
    } else {
        item->setCheckState(column,Qt::PartiallyChecked);
    }
}

void MainWindow::itemChangedSlot(QTreeWidgetItem *item, int column)
{
    if(Qt::PartiallyChecked!=item->checkState(column)){
        if(isTopItem(item) == false)
            setChildCheckState(item,item->checkState(column), column);
        else
            setParentCheckState(item, column);
    }

    if(Qt::PartiallyChecked==item->checkState(column)){
        if(!isTopItem(item)){
            item->parent()->setCheckState(column,Qt::PartiallyChecked);
        }
    }
}

void MainWindow::_clearTreeWidget(QTreeWidget *treeWidget)
{
    for(int i=0; i<treeWidget->topLevelItemCount(); i++){
        for (int j=0; j < treeWidget->topLevelItem(i)->childCount(); j++)
        {
            QTreeWidgetItem* child=treeWidget->topLevelItem(i)->child(j);
            child->setCheckState(0, Qt::Unchecked);
        }
    }
}

void
MainWindow::plotGraph(QString tables,
                      QString fields,
                      int     offsetX,
                      float   offsetY,
                      float   scale,
                      int     linestyle,
                      int     color,
                      bool    visible,
                      bool    from)
{
    bool getXSuccess = false;
    bool getYSuccess = false;
    static bool from_last = from;

    if(!from_last && from){
        clear_alreadyPloted();
        _ui.customPlot->legend->setVisible(false);
        _ui.customPlot->clearGraphs();
        _ui.customPlot->replot();
    }
    from_last = from;

    QCustomPlot* customPlot = MainWindow::getMainWindow()->ui().customPlot;

    QString plot_target = QString("%1.%2").arg(tables).arg(fields);

    if(_is_constant){
        plot_target = QString("const: %1").arg(_constant_value);
    }

    if(!_alreadyPloted.contains(plot_target))
        _alreadyPloted << plot_target;
    else
        return;

    if(visible || from){
        qCDebug(MAIN_WINDOW_LOG) << "from: "<<from<<"target: "<<plot_target;
        customPlot->addGraph();
        int length = APLDB::getAPLDB() -> getLen(tables, fields);
        QVector<double> x(length), y(length);

        getXSuccess = APLDB::getAPLDB() -> getData(tables, MainWindow::getMainWindow()->ui().comboBox->currentText(), length, x, offsetX);
        getYSuccess = APLDB::getAPLDB() -> getData(tables, fields, length, y, offsetY, scale);

        if(_is_constant){
            QVector<double> constant(length, _constant_value);
            y.swap(constant);
            _is_constant = false;
        }

        if(getXSuccess && getYSuccess){
            customPlot->graph()->setData(x, y);
        } else {
            if(!getXSuccess)
                qCDebug(MAIN_WINDOW_LOG) << "getData X Error";
            if(!getYSuccess)
                qCDebug(MAIN_WINDOW_LOG) << "getData Y Error";
        }

        customPlot->legend->setVisible(true);
        customPlot->legend->setFont(QFont("Helvetica", 9));
        customPlot->legend->setRowSpacing(-3);
        customPlot->graph()->setName(plot_target);

        _lineStyle(linestyle, color, from);

        customPlot->xAxis->setLabel(MainWindow::getMainWindow()->ui().comboBox->currentText());
        customPlot->yAxis->setLabel("y");
        customPlot->rescaleAxes();

        customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    }
}

void
MainWindow::_lineStyle(int index, int i, bool from){
    QCustomPlot* customPlot = MainWindow::getMainWindow()->ui().customPlot;
    QPen pen;

    if(from && !_conf_plot){
        int  R = 0+qrand()%(255-0);
        int  G = 0+qrand()%(255-0);
        int  B = 0+qrand()%(255-0);
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        pen.setColor(QColor(R, G, B));
        _replot = true;
    } else {
        pen.setColor(colors[0].at(i));
        _replot = false;
    }

    switch (index) {
    case 0: // Normal
        break;
    case 1: // Line1
        pen.setWidthF(2);
        break;
    case 2: // Line2
        customPlot->graph()->setBrush(QBrush(QColor(0, 0, 255, 20)));
        break;
    case 3: // Line3
        pen.setWidthF(2);
        customPlot->graph()->setBrush(QBrush(QColor(0, 0, 255, 20)));
        break;
    case 4: // Dot1
        pen.setStyle(Qt::DashLine);
        break;
    case 5: // Dot2
        pen.setStyle(Qt::DotLine);
        pen.setWidthF(2);
        break;
    case 6: // Dot3
        pen.setStyle(Qt::DashDotLine);
        customPlot->graph()->setBrush(QBrush(QColor(0, 255, 0, 20)));
        break;
    case 7: // Mark1
        pen.setWidthF(1);
        customPlot->graph()->setScatterStyle(QCPScatterStyle(shapes[0].at(0)));
        break;
    case 8: // Mark2
        pen.setWidthF(1);
        customPlot->graph()->setScatterStyle(QCPScatterStyle(shapes[0].at(1)));
        break;
    case 9: // Mark3
        customPlot->graph()->setLineStyle(QCPGraph::lsNone);
        customPlot->graph()->setScatterStyle(QCPScatterStyle(shapes[0].at(0)));
        break;
    default: // Normal
        break;
    }
    customPlot->graph()->setPen(pen);
}

bool
MainWindow::_findTable(QString table)
{
//    qCDebug(MAIN_WINDOW_LOG) << _groupName;
    if(_groupName.contains(table)){
//        qCDebug(MAIN_WINDOW_LOG) << "find table";
        return true;
    }

    qCDebug(MAIN_WINDOW_LOG) << "can not find table";
    return false;
}

bool
MainWindow::_findField(QString table, QString field)
{
    if(_is_constant) return true;

    int ItemCount = APLDB::getAPLDB() -> getItemCount(table);
    for (int j = 1; j <= ItemCount; j++)
    {
        if(APLDB::getAPLDB() -> getItemName(table, j).compare(field) == 0){
//            qCDebug(MAIN_WINDOW_LOG) << "find field";
            return true;
        }
    }

    qCDebug(MAIN_WINDOW_LOG) << "can not find field";
    return false;
}

void MainWindow::_confOpenedTrigger()
{
    QList<int> list;

    _ui.splitter->setSizes(list<<0<<1);
    _conf_plot = true;

    clear_alreadyPloted();

    plotConf(_conf);
}

void
MainWindow::plotConf(QStringList conf)
{
    QString table;
    QString field;
    QString style;
    QString color;
    QString scale("1");
    QString offsetX("0");
    QString offsetY("0");

    clearGraphNotTree();
    _plotConf = true;

    for(int i=0; i<conf.length(); i++){
        QString str(conf.at(i));
        QRegExp reg_1("[A-Za-z0-9]+\\.[A-Za-z0-9]+\\.\\d\\.\\d");
        QRegExp reg_2("[A-Za-z0-9]+\\.[A-Za-z0-9]+\\.\\d\\.\\d\\(\\-?\\d+\\.?\\d*\\s*\\,\\s*\\-?\\d+\\.?\\d*\\s*\\,\\s*\\-?\\d+\\.?\\d*\\)");
        QRegExp reg_3("\\<\\s*[A-Za-z0-9]*\\s*\\>\\s*[A-Za-Z0-9]+\\:\\-?\\d+\\.?\\d*\\s+\\d\\.\\d");
        QRegExpValidator validator_1(reg_1,0);
        QRegExpValidator validator_2(reg_2,0);
        QRegExpValidator validator_3(reg_3,0);
        bool check_ok = false;

        int pos = 0;

        if(reg_1.isValid() && !check_ok){
            switch(validator_1.validate(str,pos)){
            case QValidator::Invalid:
                qCDebug(MAIN_WINDOW_LOG)<<"reg_1 QValidator::Invalid";
                break;
            case QValidator::Intermediate:{
                qCDebug(MAIN_WINDOW_LOG)<<"reg_1 QValidator::Intermediate";
                QStringList list = str.split(".");
                table = list[0];
                field = list[1];
                if(str.count(".")==1){
                    str.append(".0.0");
                }
                break;
            }
            case QValidator::Acceptable:{
                QStringList list = str.split(".");
                table = list[0];
                field = list[1];
                style = list[2];
                color = list[3];

                check_ok = true;

                break;
            }
            }
        }

        if(reg_2.isValid() && !check_ok){
            switch(validator_2.validate(str,pos)){
            case QValidator::Invalid:
                qCDebug(MAIN_WINDOW_LOG)<<"reg_2 QValidator::Invalid";
                break;
            case QValidator::Intermediate:
                qCDebug(MAIN_WINDOW_LOG)<<"reg_2 QValidator::Intermediate";
                break;
            case QValidator::Acceptable:{
                QString table_filed_style_color(str.left(str.indexOf("(")));
                QStringList list_1 = table_filed_style_color.split(".");
                table = list_1[0];
                field = list_1[1];
                style = list_1[2];
                color = list_1[3];

                QString scale_offsetX_offsetY(str.mid(str.indexOf("(")+1, str.indexOf(")")-str.indexOf("(")-1).remove(QRegExp("\\s")));
                QStringList list = scale_offsetX_offsetY.split(",");
                scale   = list[0];
                offsetX = list[1];
                offsetY = list[2];

                check_ok = true;

                break;
                }
            }
        }

        if(reg_3.isValid() && !check_ok){
            switch(validator_3.validate(str,pos)){
            case QValidator::Invalid:
                qCDebug(MAIN_WINDOW_LOG)<<"reg_3 QValidator::Invalid";
                break;
            case QValidator::Intermediate:
                qCDebug(MAIN_WINDOW_LOG)<<"reg_3 QValidator::Intermediate";
                break;
            case QValidator::Acceptable:{
                qCDebug(MAIN_WINDOW_LOG)<<"reg_3 QValidator::Acceptable";
                QString command_str(str.simplified());
                QString command = command_str.mid(command_str.indexOf("<")+1, command_str.indexOf(">")-command_str.indexOf("<")-1);
                if(command.compare("const")==0 || command.compare("")==0){
                    QStringList list = command_str.split(QRegExp("[:\\s]"));
                    table = list[1];
                    field = MainWindow::getMainWindow()->ui().comboBox->currentText();
                    QString constant_value  = list[2];
                    QString style_color = list[3];
                    QStringList style_color_list = style_color.split(".");
                    style = style_color_list[0];
                    color = style_color_list[1];
                    _constant_value = constant_value.toDouble();

                    _is_constant = true;
                }

                check_ok = true;

                break;
            }
            }
        }

        if(_findTable(table)){
            if(_findField(table, field)){
                plotGraph(table,
                          field,
                          offsetX.toInt(),
                          offsetY.toFloat(),
                          scale.toFloat(),
                          style.toInt(),
                          color.toInt(),
                          0,
                          true);
            }
        }
    }
    _ui.customPlot->replot();

    _conf_plot = false;
}
