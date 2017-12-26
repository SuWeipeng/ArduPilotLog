#include <QDebug>
#include <QVector>
#include <QTime>
#include <QTimer>
#include "mainwindow.h"
#include "ui_mainwindow.h"

APL_LOGGING_CATEGORY(MAIN_WINDOW_LOG,        "MainWindowLog")

#define ARRAY_SIZE(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

static const char *rgDockWidgetNames[] = {
    "PID Analyze"
};

enum DockWidgetTypes {
    PID_ANALYZE
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _dialog(new Dialog)
{
    _ui.setupUi(this);
    _buildCommonWidgets();

    connect(_ui.actionOpenArduPilotLog,  &QAction::triggered, _dialog, &Dialog::showFile);

    for(int i=0;i<10;i++)
    {
        num[i] = 0;
    }
    n=0;
    QTimer *timer = new QTimer(this);
    timer->start(500);
    connect(timer,SIGNAL(timeout()),this,SLOT(Graph_Show()));
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
            case PID_ANALYZE:
                widget = (APLDockWidget*)0x01;
                break;
        }
        if(widget) {
            _mapName2DockWidget[widgetName] = widget;
        }
    }
    return widget != NULL;
}

void MainWindow::Graph_Show()
{

    QTime t;
    t=QTime::currentTime();
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    n=qrand()%50;
    Graph_Show(_ui.widget);
}

void MainWindow::Graph_Show(QCustomPlot *CustomPlot)
{
    QVector<double> temp(10);
    QVector<double> temp1(10);


    for(int i=0; i<9; i++)
    {
        num[i]=num[i+1];
    }
    num[9]=n;
    for(int i=0;i<10;i++)
    {
        temp[i] = i;
        temp1[i] =num[i];
    }
    CustomPlot->addGraph();
    CustomPlot->graph(0)->setPen(QPen(Qt::red));
    CustomPlot->graph(0)->setData(temp,temp1);

    CustomPlot->xAxis->setLabel("t");
    CustomPlot->yAxis->setLabel("mV");

    CustomPlot->xAxis->setRange(0,10);
    CustomPlot->yAxis->setRange(-50,50);
    CustomPlot->replot();
}
