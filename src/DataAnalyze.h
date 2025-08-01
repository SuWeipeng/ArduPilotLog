#ifndef DATAANALYZE_H
#define DATAANALYZE_H

#include "APLQmlWidgetHolder.h"
#include <QGuiApplication>
#include <QScreen>

class DataAnalyze : public APLQmlWidgetHolder
{
public:
    DataAnalyze(const QString& title, QAction* action, QWidget *parent = 0)
        :APLQmlWidgetHolder(title, action, parent)
    {
        Q_UNUSED(title);
        Q_UNUSED(action);
        const QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
        int screenWidth=screenGeometry.width();
        int screenHeight=screenGeometry.height();
        this->resize(screenWidth/2, screenHeight/3);
        setSource(QUrl::fromUserInput("qrc:/qml/DataAnalyze.qml"));
    }
};

#endif // DATAANALYZE_H
