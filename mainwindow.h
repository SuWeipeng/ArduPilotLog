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

public slots:
    void resizeEvent(QResizeEvent* event);

private slots:
    void _showDockWidgetAction(bool show);
    void _itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::MainWindow _ui;
    Dialog*        _dialog;

    QMap<QString, APLDockWidget*>   _mapName2DockWidget;
    QMap<QString, QAction*>         _mapName2Action;

    void _buildCommonWidgets(void);
    void _showDockWidget(const QString &name, bool show);
    bool _createInnerDockWidget(const QString& widgetName);
    void _fileOpenedTrigger();
};

#endif // MAINWINDOW_H
