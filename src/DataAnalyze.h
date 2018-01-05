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
        setSource(QUrl::fromUserInput("qrc:/qml/DataAnalyze.qml"));
    }
};

#endif // DATAANALYZE_H
