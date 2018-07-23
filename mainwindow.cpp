﻿#include <QDebug>
#include <QtQml>
#include <QTreeWidgetItem>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "src/APLRead.h"
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

extern QStringList legends;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _dialog(new Dialog)
    , _table("")
    , _field("")
    , _comboBoxListINIT(true)
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

    connect(_ui.actionOpenArduPilotLog,  &QAction::triggered, _ui.treeWidget, &QTreeWidget::clear);
    connect(_ui.actionOpenArduPilotLog,  &QAction::triggered, this, &MainWindow::_clearGraph);
    connect(_ui.actionOpenArduPilotLog,  &QAction::triggered, _dialog, &Dialog::showFile);
    connect(_ui.actionSaveDBFile,  &QAction::triggered, _dialog, &Dialog::saveFile);
    connect(_dialog->getAPLRead(),  &APLRead::fileOpened, this, &MainWindow::_fileOpenedTrigger);
    connect(_dialog,  &Dialog::saveSuccess, this, &MainWindow::_saveSuccessMessage);
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
   resizeRect.setHeight(widgetHeight*verRatio - _ui.toolBar->height() - 10);

   //set Geometry
   mainWidget->setGeometry(resizeRect);
}

void MainWindow::_fileOpenedTrigger()
{
    QTreeWidgetItem* groupItem;
    QStringList      groupName;
    int GroupCount     = APLDB::getAPLDB() -> getGroupCount();
    int ItemCount      = 0;
    int treeGroupCount = 0;

    for(int i = 1; i <= GroupCount; i++){
        if(APLDB::getAPLDB() -> isEmpty(APLDB::getAPLDB() -> getGroupName(i)) == false){
            groupName << QString("%1").arg(APLDB::getAPLDB() -> getGroupName(i));
            treeGroupCount++;
        }
    }

    groupName.sort();

    for(int i = 0; i < treeGroupCount; i++){
        QString table_name = groupName.at(i);
        groupItem = new QTreeWidgetItem(_ui.treeWidget,QStringList(table_name));
        ItemCount = APLDB::getAPLDB() -> getItemCount(table_name);
        for (int j = 1; j <= ItemCount; j++)
        {
            QTreeWidgetItem *item=new QTreeWidgetItem(groupItem,QStringList(APLDB::getAPLDB() -> getItemName(table_name, j)));
            item->setCheckState(0, Qt::Unchecked);
            groupItem->addChild(item);
        }
    }
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
    int                index;

    if(NULL==parent) return;

    index = parent->indexOfChild(item);
    _table = parent->text(column);
    _field = parent->child(index)->text(column);

    setComboboxList(_table);

    _plot2d(_ui.customPlot, _table, _field);
}

void MainWindow::_reverseHoldOn()
{
    _customPlot_hold_on = !_customPlot_hold_on;
}

void MainWindow::_clearGraph()
{
    legends.clear();
    _ui.customPlot->legend->setVisible(false);
    _ui.customPlot->clearGraphs();
    _ui.customPlot->replot();
    _clearTreeWidget(_ui.treeWidget);
}

void MainWindow::_resetGraph()
{
    _ui.customPlot->axisRect()->setRangeZoomAxes(_ui.customPlot->xAxis, _ui.customPlot->yAxis);
    _ui.customPlot->rescaleAxes();
    _ui.customPlot->replot();
}

void MainWindow::_zoomX()
{
    _ui.customPlot->axisRect()->setRangeZoomAxes(_ui.customPlot->xAxis, NULL);
}

void MainWindow::_zoomY()
{
    _ui.customPlot->axisRect()->setRangeZoomAxes(NULL, _ui.customPlot->yAxis);
}

void MainWindow::on_customPlot_customContextMenuRequested()
{
    QMenu *menu=new QMenu(_ui.customPlot);

    // Hold on
    QAction* pHoldOn = new QAction(tr("Hold On"), this);
    connect(pHoldOn, &QAction::triggered, this, &MainWindow::_reverseHoldOn);
    menu->addAction(pHoldOn);
    // Clear graph
    QAction* pClearGraph = new QAction(tr("Clear"), this);
    connect(pClearGraph, &QAction::triggered, this, &MainWindow::_clearGraph);
    menu->addAction(pClearGraph);
    // Reset graph
    QAction* pResetGraph = new QAction(tr("Set to default"), this);
    connect(pResetGraph, &QAction::triggered, this, &MainWindow::_resetGraph);
    menu->addAction(pResetGraph);
    // Zoom X
    QAction* pZoomX = new QAction(tr("zoom X"), this);
    connect(pZoomX, &QAction::triggered, this, &MainWindow::_zoomX);
    menu->addAction(pZoomX);
    // Zoom Y
    QAction* pZoomY = new QAction(tr("zoom Y"), this);
    connect(pZoomY, &QAction::triggered, this, &MainWindow::_zoomY);
    menu->addAction(pZoomY);

    menu->exec(QCursor::pos());
}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    if(arg1.compare("") == 0) return;
    _X_axis_changed = true;
    _comboBoxIndex  = _ui.comboBox->currentIndex();
    legends.clear();
    _ui.customPlot->legend->setVisible(false);
    _ui.customPlot->clearGraphs();
    _ui.customPlot->xAxis->setLabel(MainWindow::getMainWindow()->ui().comboBox->currentText());
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
    for (int i=0;i<childCount;i++)
    {
        QTreeWidgetItem* child= item->child(i);
        if(child->checkState(column)==Qt::Checked)
        {
            selectedCount++;
            _plotGraph(item->child(i), column);
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
    Q_UNUSED(treeWidget)
    // TODO
}
