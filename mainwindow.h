#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_MainWindow.h"

#include "APLDockWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void _showDockWidgetAction(bool show);

private:
    Ui::MainWindow _ui;

    QMap<QString, APLDockWidget*>   _mapName2DockWidget;
    QMap<QString, QAction*>         _mapName2Action;

    void _buildCommonWidgets(void);
    void _showDockWidget(const QString &name, bool show);
    bool _createInnerDockWidget(const QString& widgetName);
};

#endif // MAINWINDOW_H
