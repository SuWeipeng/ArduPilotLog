#include <QDebug>
#include <QtQml>
#include <QGuiApplication>
#include <QScreen>
#include <QTreeWidgetItem>
#include <QRandomGenerator>
#include <QSharedPointer>
#include <QMenu>
#include <QWidgetAction>
#include <QCheckBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "src/APLRead.h"
#include "src/APLReadConf.h"
#include "src/APLDataCache.h"
#include "src/DataAnalyze.h"
#include "src/DataAnalyzeController.h"

APL_LOGGING_CATEGORY(MAIN_WINDOW_LOG,        "MainWindowLog")

#define ARRAY_SIZE(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

bool        MainWindow::_customPlot_hold_on;
bool        MainWindow::_X_axis_changed;
MainWindow* MainWindow::_instance;

static const char *rgDockWidgetNames[] = {
    "Data Analyze"
};

enum DockWidgetTypes {
    DATA_ANALYZE
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _dialog(new Dialog)
    , _dialog_load(new DialogLoad)
    , _mTracerLine(nullptr)
    , _mTracerText(nullptr)
    , _mIsTracerEnabled(false)
    , _table("")
    , _field("")
    , _conf_plot(false)
    , _is_constant(false)
    , _replot(false)
    , _plotConf(false)
    , _action_bold(0x1<<0)
    , _color_idx(-1)
{
    qmlRegisterType<DataAnalyzeController>("ArduPilotLog.Controllers", 1, 0, "DataAnalyzeController");

    const QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    int screenWidth=screenGeometry.width();
    int screenHeight=screenGeometry.height();
    _ui.setupUi(this);
    this->resize(screenWidth/2, screenHeight/2);
    _buildCommonWidgets();
    _ui.splitter->setStretchFactor(0, 2);
    _ui.splitter->setStretchFactor(1, 8);
    _ui.progressBar->setVisible(0);
    _ui.progressBar->setValue(0);
    _ui.progressBar->setRange(0,9999);
    initTreeWidget();
    _instance = this;

    QCustomPlot* customPlot = MainWindow::getMainWindow()->ui().customPlot;

    // ==================== 跟踪器初始化 - 开始 ====================

    // 1. 创建竖线 mTracerLine
    _mTracerLine = new QCPItemStraightLine(customPlot);
    _mTracerLine->setPen(QPen(Qt::red, 1, Qt::DashLine)); // 设置画笔为红色虚线
    _mTracerLine->setVisible(false); // 初始时不可见
    _mFixedLines.clear();
    _mFixedTexts.clear();
    _x_us.clear();

    // 2. 创建文本标签 mTracerText
    _mTracerText = new QCPItemText(customPlot);
    _mTracerText->setLayer("overlay"); // 放置在最上层
    _mTracerText->setPen(QPen(Qt::black));
    _mTracerText->setBrush(QColor(240, 240, 240, 200)); // 半透明背景
    _mTracerText->setPadding(QMargins(5, 5, 5, 5));
    _mTracerText->setVisible(false); // 初始时不可见

    // 设置文本位置：锚定在图表区域的顶部中心
    _mTracerText->position->setType(QCPItemPosition::ptAxisRectRatio);
    _mTracerText->position->setCoords(0.5, 0.05); // x=0.5 (中心), y=0.05 (靠近顶部)
    _mTracerText->setTextAlignment(Qt::AlignTop | Qt::AlignHCenter);
    _mTracerText->setFont(QFont(font().family(), 10));

    // 3. 连接 mouseMove 信号到我们的槽函数
    connect(customPlot, &QCustomPlot::mouseMove, this, &MainWindow::_onMouseMove);
    connect(customPlot, &QCustomPlot::mousePress, this, &MainWindow::_onMousePress);

    // ==================== 跟踪器初始化 - 结束 ====================

    // ==================== 创建带复选框的菜单项 - 开始 ====================

    // 1. 创建一个可勾选的 QAction
    QAction *tracerAction = new QAction("Show TimeUS", this);
    tracerAction->setCheckable(true);
    tracerAction->setChecked(false);

    // 2将这个 action 添加到 "Tools" 菜单
    _ui.menuTools->addAction(tracerAction);

    // 3. 连接 QCheckBox 的 toggled 信号到我们创建的槽
    connect(tracerAction, &QAction::toggled, this, &MainWindow::_onTracerToggled);

    // ==================== 创建带复选框的菜单项 - 结束 ====================

    // ==================== 创建带复选框的菜单项 - 开始 ====================

    // 1. 创建一个可勾选的 QAction
    QAction *splitAction = new QAction("Split Table", this);
    splitAction->setCheckable(true);
    splitAction->setChecked(false);

    // 2将这个 action 添加到 "Tools" 菜单
    _ui.menuTools->addAction(splitAction);

    // 3. 连接 QCheckBox 的 toggled 信号到我们创建的槽
    connect(splitAction, &QAction::toggled, _dialog, &Dialog::split);

    // ==================== 创建带复选框的菜单项 - 结束 ====================

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
    connect(_ui.actionExportCSV,  &QAction::triggered, _dialog->getAPLRead(), &APLRead::exportCSV);
    connect(_ui.actionTrim,  &QAction::triggered, _dialog, &Dialog::trim);
    connect(_dialog->getAPLRead(),  &APLRead::fileOpened, this, &MainWindow::_fileOpenedTrigger);
    connect(_dialog_load->getAPLReadConf(),  &APLReadConf::fileOpened, this, &MainWindow::_confOpenedTrigger);
    connect(_dialog,  &Dialog::saveSuccess, this, &MainWindow::_saveSuccessMessage);
    connect(_dialog->getAPLRead()->export_worker,  &APLExportWorker::saveSuccess, this, &MainWindow::_saveSuccessMessage);
    emit(_ui.actionOpenArduPilotLog->triggered());
}

MainWindow::~MainWindow()
{
    delete _dialog;
    delete _dialog_load;
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
    int GroupCount     = APLDataCache::get_singleton()->getTableNum();
    int ItemCount      = 0;
    int treeGroupCount = 0;

    _groupName.clear();

    for(int i = 0; i < GroupCount; i++){
        _groupName << QString("%1").arg(APLDataCache::get_singleton()->getTableName(i));
        treeGroupCount++;
    }

    for(int i = 0; i < treeGroupCount; i++){
        QString table_name = _groupName.at(i);
        groupItem = new QTreeWidgetItem(_ui.treeWidget,QStringList(table_name));
        ItemCount = APLDataCache::get_singleton()->getItemCount(table_name);
        for (int j = 0; j < ItemCount; j++)
        {
            QTreeWidgetItem *item=new QTreeWidgetItem(groupItem,QStringList(APLDataCache::get_singleton()->getItemName(table_name, j)));
            item->setCheckState(0, Qt::Unchecked);
            groupItem->addChild(item);
        }
    }

    requestTableList();

    QMap<QString, QStringList> data;
    for(int i = 0; i < _ui.treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* tableItem = _ui.treeWidget->topLevelItem(i);
        QString tableName = tableItem->text(0);
        QStringList fields;
        for (int j = 0; j < tableItem->childCount(); ++j) {
            fields << tableItem->child(j)->text(0);
        }
        data.insert(tableName, fields);
    }
    emit dataReady(data);
}

void MainWindow::requestTableList()
{
    for(int i=0; i<_ui.treeWidget->topLevelItemCount(); i++){
        emit treeWidgetAddItem(_ui.treeWidget->topLevelItem(i)->text(0));
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

    QString remove_target = QString("%1.%2").arg(_table, _field);

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
    clearFixedMarkers();
    _ui.customPlot->legend->setVisible(false);
    _ui.customPlot->clearGraphs();
    _ui.customPlot->replot();
    _clearTreeWidget(_ui.treeWidget);
}

void MainWindow::clearGraphNotTree()
{
    clear_alreadyPloted();
    clearFixedMarkers();
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
    menu->setAttribute(Qt::WA_DeleteOnClose);

    // Clear graph
    QAction* pClearGraph = new QAction(tr("Clear"), menu);
    connect(pClearGraph, &QAction::triggered, this, &MainWindow::clearGraph);
    menu->addAction(pClearGraph);
    // Reset graph
    QAction* pResetGraph = new QAction(tr("Reset graph"), menu);
    ft.setBold((_action_bold & 0x1) != 0);
    pResetGraph->setFont(ft);
    connect(pResetGraph, &QAction::triggered, this, &MainWindow::_resetGraph);
    menu->addAction(pResetGraph);
    // Zoom X
    QAction* pZoomX = new QAction(tr("zoom X"), menu);
    ft.setBold((_action_bold & 0x2) != 0);
    pZoomX->setFont(ft);
    connect(pZoomX, &QAction::triggered, this, &MainWindow::_zoomX);
    menu->addAction(pZoomX);
    // Zoom Y
    QAction* pZoomY = new QAction(tr("zoom Y"), menu);
    ft.setBold((_action_bold & 0x4) != 0);
    pZoomY->setFont(ft);
    connect(pZoomY, &QAction::triggered, this, &MainWindow::_zoomY);
    menu->addAction(pZoomY);
    // Zoom All
    QAction* pZoomAll = new QAction(tr("Zoom All"), menu);
    ft.setBold((_action_bold & 0x8) != 0);
    pZoomAll->setFont(ft);
    connect(pZoomAll, &QAction::triggered, this, &MainWindow::_zoomAll);
    menu->addAction(pZoomAll);
    menu->exec(QCursor::pos());
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
                      double  offsetY,
                      double  scale,
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
        _color_idx = -1;
    }
    from_last = from;

    QCustomPlot* customPlot = MainWindow::getMainWindow()->ui().customPlot;

    QString plot_target = QString("%1.%2").arg(tables, fields);

    if(_is_constant){
        plot_target = QString("const: %1").arg(_constant_value);
    }

    if(!_alreadyPloted.contains(plot_target))
        _alreadyPloted << plot_target;
    else
        return;

    if(visible || from){
        if (from) {
            _color_idx++;
        }
        qCDebug(MAIN_WINDOW_LOG) << "from: "<<from<<"target: "<<plot_target;
        customPlot->addGraph();
        int length = APLDataCache::get_singleton()->getLen(tables, fields);
        QVector<double> x_us(length), y(length);

        getXSuccess = APLDataCache::get_singleton()->getData(tables, QString("TimeUS"), length, x_us, offsetX);
        getYSuccess = APLDataCache::get_singleton()->getData(tables, fields, length, y, offsetY, scale);

        if(_is_constant){
            QVector<double> constant(length, _constant_value);
            y.swap(constant);
            _is_constant = false;
        }

        if(getXSuccess && getYSuccess){
            QVector<double> x_seconds(length);
            for (int i = 0; i < length; ++i) {
                x_seconds[i] = x_us[i] / 1000000.0;
            }
            customPlot->graph()->setData(x_seconds, y);
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

        // === 修改开始: 动态时间刻度格式化 ===

        // 1. 确保 x 轴的 Ticker 是 QCPAxisTickerTime 类型
        //    我们每次都创建一个新的，以确保类型正确
        QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
        customPlot->xAxis->setTicker(timeTicker);

        // 2. 重新计算所有坐标轴的范围
        customPlot->rescaleAxes();

        // 3. 获取 x 轴范围并决定时间格式
        QCPRange xRange = customPlot->xAxis->range();
        double durationInSeconds = xRange.upper - xRange.lower;
        const double secondsInHour = 3600.0;

        if (durationInSeconds < secondsInHour) {
            // 如果总时长小于1小时，使用 "分钟:秒" 格式
            timeTicker->setTimeFormat("%m:%s.%z");
            customPlot->xAxis->setLabel("Time (Min:S.MS)");
        } else {
            // 如果总时长大于等于1小时，使用 "小时:分钟:秒" 格式
            timeTicker->setTimeFormat("%h:%m:%s.%z");
            customPlot->xAxis->setLabel("Time (H:Min:S.MS)");
        }

        customPlot->yAxis->setLabel("y");

        // === 修改结束 ===

        customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    }
}

void
MainWindow::_lineStyle(int index, int i, bool from){
    QCustomPlot* customPlot = MainWindow::getMainWindow()->ui().customPlot;
    QPen pen;

    if(from && !_conf_plot){
        static QStringList predefinedColors = {"Red","Green","Blue","Magenta","LawnGreen","Pink","DeepSkyBlue","Orange","DarkCyan","Gold"};

        if (_color_idx < 10) {
            pen.setColor(QColor(predefinedColors[_color_idx]));
        } else {
            int  R = QRandomGenerator::global()->bounded(255);
            int  G = QRandomGenerator::global()->bounded(255);
            int  B = QRandomGenerator::global()->bounded(255);
            pen.setColor(QColor(R, G, B));
        }
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

    int ItemCount = APLDataCache::get_singleton()->getItemCount(table);
    for (int j = 1; j <= ItemCount; j++)
    {
        if(APLDataCache::get_singleton()->getItemName(table, j).compare(field) == 0){
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

void MainWindow::_onMouseMove(QMouseEvent *event)
{
    if (!_mIsTracerEnabled) {
        return;
    }

    QCustomPlot* customPlot = MainWindow::getMainWindow()->ui().customPlot;

    // 检查鼠标是否在图表区域内
    if (customPlot->axisRect()->rect().contains(event->pos()))
    {
        // 1. 将鼠标的像素位置转换为 x 轴的坐标值 (单位：秒)
        double x_coord = customPlot->xAxis->pixelToCoord(event->pos().x());

        // 2. 更新竖线的位置
        _mTracerLine->point1->setCoords(x_coord, customPlot->yAxis->range().lower);
        _mTracerLine->point2->setCoords(x_coord, customPlot->yAxis->range().upper);
        _mTracerLine->setVisible(true);

        // 3. 将秒转换为微秒用于显示
        double x_us = x_coord * 1000000.0;

        // 4. 更新文本标签的内容和位置
        _mTracerText->setText(QString::number(x_us, 'f', 0) + " us");
        _mTracerText->position->setCoords(x_coord, customPlot->yAxis->range().upper * 0.9); // 文字位置跟随红线
        _mTracerText->setVisible(true);

        // 5. 重新绘制图表以显示更新
        customPlot->replot();
    }
    else
    {
        // 如果鼠标移出图表区域，则隐藏跟踪器
        if (_mTracerLine->visible() || _mTracerText->visible())
        {
            _mTracerLine->setVisible(false);
            _mTracerText->setVisible(false);
            customPlot->replot();
        }
    }
}

void MainWindow::_onMousePress(QMouseEvent *event)
{
    if (!_mIsTracerEnabled || event->button() != Qt::LeftButton) {
        return;
    }

    QCustomPlot* customPlot = MainWindow::getMainWindow()->ui().customPlot;

    if (customPlot->axisRect()->rect().contains(event->pos()))
    {
        double x_coord = customPlot->xAxis->pixelToCoord(event->pos().x());
        double x_us = x_coord * 1000000.0;

        // 创建固定的竖线 - 设置为虚线样式
        QCPItemStraightLine* fixedLine = new QCPItemStraightLine(customPlot);
        fixedLine->point1->setCoords(x_coord, customPlot->yAxis->range().lower);
        fixedLine->point2->setCoords(x_coord, customPlot->yAxis->range().upper);

        // 关键修改：设置为红色虚线，与跟踪线保持一致
        fixedLine->setPen(QPen(Qt::red, 1, Qt::DashLine));

        // 其余代码保持不变...
        QCPItemText* fixedText = new QCPItemText(customPlot);
        fixedText->position->setCoords(x_coord, customPlot->yAxis->range().upper * 0.9);
        fixedText->setText(QString::number(x_us, 'f', 0) + " us");
        fixedText->setFont(QFont("Arial", 10));
        fixedText->setColor(Qt::red);
        fixedText->setPadding(QMargins(5, 5, 5, 5));
        fixedText->setBrush(QBrush(QColor(255, 255, 255, 200)));

        _mFixedLines.append(fixedLine);
        _mFixedTexts.append(fixedText);
        _x_us.append(qRound64(x_us));

        if (_mFixedLines.length() > 2) {
            customPlot->removeItem(_mFixedLines.front());
            customPlot->removeItem(_mFixedTexts.front());
            _mFixedLines.pop_front();
            _mFixedTexts.pop_front();
            _x_us.pop_front();
        }
        if (_mFixedLines.length() == 2) {
            quint8 minIdx = 0;
            quint8 maxIdx = 1;
            if(_x_us.first() > _x_us.last()) {
                minIdx = 1;
                maxIdx = 0;
            }
            _dialog->setTrimFrom(_x_us.at(minIdx));
            _dialog->setTrimTo(_x_us.at(maxIdx));
        }

        customPlot->replot();
    }
}

void MainWindow::clearFixedMarkers()
{
    QCustomPlot* customPlot = MainWindow::getMainWindow()->ui().customPlot;

    // 清除所有固定的线条
    for (QCPItemStraightLine* line : _mFixedLines) {
        customPlot->removeItem(line);
    }
    _mFixedLines.clear();

    // 清除所有固定的文本
    for (QCPItemText* text : _mFixedTexts) {
        customPlot->removeItem(text);
    }
    _mFixedTexts.clear();

    _x_us.clear();

    customPlot->replot();
}

void MainWindow::_onTracerToggled(bool checked)
{
    // 1. 更新我们的状态标志
    _mIsTracerEnabled = checked;

    // 2. 如果是取消勾选 (checked 为 false)，则立即隐藏跟踪器
    if (!checked) {
        clearFixedMarkers();
        if (_mTracerLine && _mTracerText) {
            _mTracerLine->setVisible(false);
            _mTracerText->setVisible(false);
            _ui.customPlot->replot();
        }
    }
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
        QRegularExpression reg_1("[A-Za-z0-9]+\\.[A-Za-z0-9]+\\.\\d\\.\\d");
        QRegularExpression reg_2("[A-Za-z0-9]+\\.[A-Za-z0-9]+\\.\\d\\.\\d\\(\\-?\\d+\\.?\\d*\\s*\\,\\s*\\-?\\d+\\.?\\d*\\s*\\,\\s*\\-?\\d+\\.?\\d*\\)");
        QRegularExpression reg_3("\\<\\s*[A-Za-z0-9]*\\s*\\>\\s*[A-Za-Z0-9]+\\:\\-?\\d+\\.?\\d*\\s+\\d\\.\\d");
        QRegularExpressionValidator validator_1(reg_1,0);
        QRegularExpressionValidator validator_2(reg_2,0);
        QRegularExpressionValidator validator_3(reg_3,0);
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

                QString scale_offsetX_offsetY(str.mid(str.indexOf("(")+1, str.indexOf(")")-str.indexOf("(")-1).remove(QRegularExpression("\\s")));
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
                    QStringList list = command_str.split(QRegularExpression("[:\\s]"));
                    table = list[1];
                    field = QString("TimeUS");
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
