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
    , _table("")
    , _field("")
    , _comboBoxListINIT(true)
    , _action_bold(0x1<<0)
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
    connect(_ui.actionSaveDBFile,  &QAction::triggered, _dialog, &Dialog::saveFile);
    connect(_dialog->getAPLRead(),  &APLRead::fileOpened, this, &MainWindow::_fileOpenedTrigger);
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
    _ui.customPlot->legend->setVisible(false);
    _ui.customPlot->clearGraphs();
    _ui.customPlot->replot();
    _clearTreeWidget(_ui.treeWidget);
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

void MainWindow::on_customPlot_customContextMenuRequested()
{
    QFont ft;
    QMenu *menu=new QMenu(_ui.customPlot);

    // Clear graph
    QAction* pClearGraph = new QAction(tr("Clear"), this);
    connect(pClearGraph, &QAction::triggered, this, &MainWindow::clearGraph);
    menu->addAction(pClearGraph);
    // Reset graph
    QAction* pResetGraph = new QAction(tr("Set to default"), this);
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
    QCustomPlot* customPlot = MainWindow::getMainWindow()->ui().customPlot;

    QString plot_target = QString("%1.%2").arg(tables).arg(fields);

    qCDebug(MAIN_WINDOW_LOG) << "from: "<<from<<"target: "<<plot_target;

    if(!_alreadyPloted.contains(plot_target))
        _alreadyPloted << plot_target;
    else
        return;

    if(visible || from){
        customPlot->addGraph();
        int length = APLDB::getAPLDB() -> getLen(tables, fields);
        QVector<double> x(length), y(length);

        getXSuccess = APLDB::getAPLDB() -> getData(tables, MainWindow::getMainWindow()->ui().comboBox->currentText(), length, x, offsetX);
        getYSuccess = APLDB::getAPLDB() -> getData(tables, fields, length, y, offsetY, scale);
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

    if(from){
        int  R = 0+qrand()%(255-0);
        int  G = 0+qrand()%(255-0);
        int  B = 0+qrand()%(255-0);
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        pen.setColor(QColor(R, G, B));
    } else {
        pen.setColor(colors[0].at(i));
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
