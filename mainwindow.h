#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_MainWindow.h"

#include "APLLoggingCategory.h"
#include "APLDockWidget.h"
#include "src/Dialog.h"
#include "qcustomplot.h"

Q_DECLARE_LOGGING_CATEGORY(MAIN_WINDOW_LOG)

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static bool get_customPlot_hold_on() { return _customPlot_hold_on; }

public slots:
    void resizeEvent(QResizeEvent* event);

private slots:
    void _showDockWidgetAction(bool show);
    void _itemClicked(QTreeWidgetItem *item, int column);
    void _reverseHoldOn();

    void on_customPlot_customContextMenuRequested();

private:
    Ui::MainWindow _ui;
    Dialog*        _dialog;
    static bool    _customPlot_hold_on;

    QMap<QString, APLDockWidget*>   _mapName2DockWidget;
    QMap<QString, QAction*>         _mapName2Action;

    void _buildCommonWidgets(void);
    void _showDockWidget(const QString &name, bool show);
    bool _createInnerDockWidget(const QString& widgetName);
    void _fileOpenedTrigger();
    void _plot2d(QCustomPlot *customPlot, QString& table, QString& field);
};

#endif // MAINWINDOW_H
