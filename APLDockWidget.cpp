/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


#include "APLDockWidget.h"

#include <QCloseEvent>
#include <QSettings>

const char*  APLDockWidget::_settingsGroup = "DockWidgets";

APLDockWidget::APLDockWidget(const QString& title, QAction* action, QWidget* parent)
    : QWidget(parent)
    , _title(title)
	, _action(action)
{
    if (action) {
        setWindowTitle(title);
        setWindowFlags(Qt::Tool);
        loadSettings();
    }
}

// Instead of destroying the widget just hide it
void APLDockWidget::closeEvent(QCloseEvent* event)
{
    if (_action) {
        saveSettings();
        event->ignore();
        _action->trigger();
    } else {
        QWidget::closeEvent(event);
    }
}

void APLDockWidget::loadSettings(void)
{
    // TODO: This is crashing for some reason. Disabled until sorted out.
    if (0 /*_action*/) {
        QSettings settings;
        settings.beginGroup(_settingsGroup);
        if (settings.contains(_title)) {
            restoreGeometry(settings.value(_title).toByteArray());
        }
        settings.endGroup();
    }
}

void APLDockWidget::saveSettings(void)
{
    // TODO: This is crashing for some reason. Disabled until sorted out.
    if (0 /*_action*/) {
        QSettings settings;
        settings.beginGroup(_settingsGroup);
        settings.setValue(_title, saveGeometry());
        settings.endGroup();
    }
}
