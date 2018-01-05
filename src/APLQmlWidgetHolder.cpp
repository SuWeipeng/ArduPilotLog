#include <QDesktopWidget>
#include "APLQmlWidgetHolder.h"

APLQmlWidgetHolder::APLQmlWidgetHolder(const QString& title, QAction* action, QWidget *parent) :
    APLDockWidget(title, action, parent)
{
    _ui.setupUi(this);

    int screenWidth=QApplication::desktop()->width();
    int screenHeight=QApplication::desktop()->height();
    this->resize(screenWidth/4, screenHeight/4);

    layout()->setContentsMargins(0,0,0,0);

    if (action) {
        setWindowTitle(title);
    }
    setResizeMode(QQuickWidget::SizeRootObjectToView);
}

APLQmlWidgetHolder::~APLQmlWidgetHolder()
{

}

void APLQmlWidgetHolder::setSource(const QUrl& qmlUrl)
{
    _ui.qmlWidget->setSource(qmlUrl);
}

void APLQmlWidgetHolder::setContextPropertyObject(const QString& name, QObject* object)
{
    _ui.qmlWidget->rootContext()->setContextProperty(name, object);
}

QQmlContext* APLQmlWidgetHolder::getRootContext(void)
{
    return _ui.qmlWidget->rootContext();
}

QQuickItem* APLQmlWidgetHolder::getRootObject(void)
{
    return _ui.qmlWidget->rootObject();
}

QQmlEngine*	APLQmlWidgetHolder::getEngine()
{
    return _ui.qmlWidget->engine();
}


void APLQmlWidgetHolder::setResizeMode(QQuickWidget::ResizeMode resizeMode)
{
    _ui.qmlWidget->setResizeMode(resizeMode);
}
