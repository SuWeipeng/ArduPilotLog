#ifndef APLQmlWidgetHolder_h
#define APLQmlWidgetHolder_h

#include <QWidget>
#include <QQmlContext>
#include <QQmlEngine>
#include "APLDockWidget.h"
#include "ui_APLQmlWidgetHolder.h"

namespace Ui {
class APLQmlWidgetHolder;
}

/// This is used to create widgets which are implemented in QML.

class APLQmlWidgetHolder : public APLDockWidget
{
    Q_OBJECT

public:
    // This has a title and action since the base class is APLQmlWidget. In order to use this
    // control as a normal QWidget, not a doc widget just pass in:
    //      title = QString()
    //      action = NULL
    explicit APLQmlWidgetHolder(const QString& title, QAction* action, QWidget *parent = 0);
    ~APLQmlWidgetHolder();

    /// Get Root Context
    QQmlContext* getRootContext(void);

    /// Get Root Object
    QQuickItem* getRootObject(void);

    /// Get QML Engine
    QQmlEngine*	getEngine();

    void setSource(const QUrl& qmlUrl);

    void setContextPropertyObject(const QString& name, QObject* object);

    /// Sets the resize mode for the QQuickWidget container
    void setResizeMode(QQuickWidget::ResizeMode resizeMode);

private:
    Ui::APLQmlWidgetHolder _ui;
};

#endif
