#ifndef DATAANALYZE_H
#define DATAANALYZE_H

#include "APLQmlWidgetHolder.h"

class DataAnalyze : public APLQmlWidgetHolder
{
public:
    DataAnalyze(const QString& title, QAction* action, QWidget *parent = 0)
        :APLQmlWidgetHolder(title, action, parent)
    {
        Q_UNUSED(title);
        Q_UNUSED(action);
        int screenWidth=QApplication::desktop()->width();
        int screenHeight=QApplication::desktop()->height();
        this->resize(screenWidth/2, screenHeight/3);
        setSource(QUrl::fromUserInput("qrc:/qml/DataAnalyze.qml"));
    }
};

#endif // DATAANALYZE_H
