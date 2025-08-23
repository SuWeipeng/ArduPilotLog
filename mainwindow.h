#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QStringList>
#include <QSqlDatabase>
#include "ui_mainwindow.h"

#include "APLLoggingCategory.h"
#include "APLDockWidget.h"
#include "src/Dialog.h"
#include "src/DialogLoad.h"
#include "qcustomplot.h"

Q_DECLARE_LOGGING_CATEGORY(MAIN_WINDOW_LOG)

class QCheckBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static bool        get_X_axis_changed()       { return _X_axis_changed; }
    static void        set_X_axis_changed(bool b) { _X_axis_changed = b; }
    static MainWindow* getMainWindow()            { return _instance; }
    Ui::MainWindow&    ui()                       { return _ui; }
    QSqlDatabase&      db()                       { return _apldb; }
    void        requestTableList();
    void        closeEvent(QCloseEvent * event);

    void initTreeWidget();
    bool isTopItem(QTreeWidgetItem* item);
    void setChildCheckState(QTreeWidgetItem *item, Qt::CheckState cs, int column);
    void setParentCheckState(QTreeWidgetItem *item, int column);
    void set_conf(QStringList conf) { _conf = conf; }

    QVector<QCPScatterStyle::ScatterShape> shapes[10];
    QVector<QColor>                        colors[10];

public slots:
    void resizeEvent(QResizeEvent* event);
    void itemChangedSlot(QTreeWidgetItem* item, int column);
    void plotGraph(QString tables,
                   QString fields,
                   int     offsetX,
                   double  offsetY,
                   double  scale,
                   int     linestyle,
                   int     color,
                   bool    visible,
                   bool    from);    // false:DataAnalyzeController,true:Other
    void clear_alreadyPloted() { _alreadyPloted.clear(); }
    void clearGraph();
    void clearGraphNotTree();
    void clearFixedMarkers();
    void plotConf(QStringList conf);

private slots:
    void _showDockWidgetAction(bool show);
    void _plotGraph(QTreeWidgetItem *item, int column);
    void _removeGraph(QTreeWidgetItem *item, int column);
    void _resetGraph();
    void _zoomX();
    void _zoomY();
    void _zoomAll();
    void on_customPlot_customContextMenuRequested();
    void _saveSuccessMessage();
    void _confOpenedTrigger();
    void _onMouseMove(QMouseEvent *event);
    void _onMousePress(QMouseEvent *event);
    void _onTracerToggled(bool checked);

signals:
    void treeWidgetAddItem(QString name);
    void tableListReady();
    void dataReady(QMap<QString, QStringList> data);

private:
    Ui::MainWindow      _ui;
    Dialog*             _dialog;
    DialogLoad*         _dialog_load;
    QCPItemStraightLine* _mTracerLine;
    QCPItemText*        _mTracerText;
    bool                _mIsTracerEnabled;
    QList<QCPItemStraightLine*> _mFixedLines;
    QList<QCPItemText*> _mFixedTexts;
    static bool         _customPlot_hold_on;
    static bool         _X_axis_changed;
    static MainWindow*  _instance;
    QString             _table;
    QString             _field;
    QStringList         _alreadyPloted;
    QStringList         _groupName;
    QStringList         _conf;
    bool                _conf_plot;
    bool                _is_constant;
    bool                _replot;
    bool                _plotConf;
    double              _constant_value;
    int                 _action_bold;
    QSqlDatabase        _apldb;

    QMap<QString, APLDockWidget*>   _mapName2DockWidget;
    QMap<QString, QAction*>         _mapName2Action;

    void _buildCommonWidgets(void);
    void _showDockWidget(const QString &name, bool show);
    bool _createInnerDockWidget(const QString& widgetName);
    void _fileOpenedTrigger();
    void _clearTreeWidget(QTreeWidget *treeWidget);
    void _lineStyle(int index, int i, bool from);
    bool _findTable(QString table);
    bool _findField(QString table, QString field);
};

#endif // MAINWINDOW_H
