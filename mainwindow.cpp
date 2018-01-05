#include <QDebug>
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

bool MainWindow::_customPlot_hold_on;
int  MainWindow::_comboBoxIndex;
bool MainWindow::_X_axis_changed;

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
{
    qmlRegisterType<DataAnalyzeController>("ArduPilotLog.Controllers", 1, 0, "DataAnalyzeController");

    _ui.setupUi(this);
    _buildCommonWidgets();
    _ui.splitter->setStretchFactor(0, 1);
    _ui.splitter->setStretchFactor(1, 8);
    _ui.treeWidget->setColumnCount(1);
    _ui.treeWidget->setHeaderLabel(tr("Log"));

    connect(_ui.actionOpenArduPilotLog,  &QAction::triggered, _ui.treeWidget, &QTreeWidget::clear);
    connect(_ui.actionOpenArduPilotLog,  &QAction::triggered, this, &MainWindow::_clearGraph);
    connect(_ui.actionOpenArduPilotLog,  &QAction::triggered, _dialog, &Dialog::showFile);
    connect(_dialog->getAPLRead(),  &APLRead::fileOpened, this, &MainWindow::_fileOpenedTrigger);
    connect(_ui.treeWidget, &QTreeWidget::itemClicked, this,&MainWindow::_itemClicked);
}

MainWindow::~MainWindow()
{
    delete _dialog;
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
    int GroupCount = APLDB::getAPLDB() -> getGroupCount();
    int ItemCount  = 0;

    for(int i = 1; i <= GroupCount; i++){
        groupName << QString("%1").arg(APLDB::getAPLDB() -> getGroupName(i));
    }

    groupName.sort();

    for(int i = 0; i < GroupCount; i++){
        QString table_name = groupName.at(i);
        groupItem = new QTreeWidgetItem(_ui.treeWidget,QStringList(table_name));
        ItemCount = APLDB::getAPLDB() -> getItemCount(table_name);
        for (int j = 1; j <= ItemCount; j++)
        {
            QTreeWidgetItem *item=new QTreeWidgetItem(groupItem,QStringList(APLDB::getAPLDB() -> getItemName(table_name, j)));
            groupItem->addChild(item);
        }
    }
}

void MainWindow::_itemClicked(QTreeWidgetItem *item, int column)
{
    QTreeWidgetItem*   parent = item->parent();
    int                index;
    static QStringList conboBoxList;
    QString            Item0;
    QString            Item1;
    static bool        initialize = true;

    if(NULL==parent) return;

    index = parent->indexOfChild(item);
    _table = parent->text(column);
    _field = parent->child(index)->text(column);

    //comboBox
    Item0 = APLDB::getAPLDB()->getItemName(_table, 0);
    Item1 = APLDB::getAPLDB()->getItemName(_table, 1);
    if(!conboBoxList.contains(Item0)){
        conboBoxList<<Item0;
        _ui.comboBox->addItem(Item0);
    }
    if(!conboBoxList.contains(Item1)){
        conboBoxList<<Item1;
        _ui.comboBox->addItem(Item1);
    }
    if(initialize){
        initialize = false;
        _ui.comboBox->setCurrentIndex(1);
    }

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
    _ui.customPlot->xAxis->setLabel(APLDB::getAPLDB()->getItemName(_table, get_comboBoxIndex()));
    _ui.customPlot->replot();
}
