#include <QDebug>
#include <QTreeWidgetItem>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "src/APLRead.h"
#include "src/APLDB.h"

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
    _ui.splitter->setStretchFactor(0, 1);
    _ui.splitter->setStretchFactor(1, 9);
    _ui.treeWidget->setColumnCount(1);
    _ui.treeWidget->setHeaderLabel(tr("Log"));

    connect(_ui.actionOpenArduPilotLog,  &QAction::triggered, _dialog, &Dialog::showFile);
    connect(_dialog->getAPLRead(),  &APLRead::fileOpened, this, &MainWindow::_fileOpenedTrigger);
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
