#ifndef APLDB_H
#define APLDB_H

#include <QObject>
#include "APLLoggingCategory.h"

Q_DECLARE_LOGGING_CATEGORY(APLDB_LOG)

class APLDB : public QObject
{
    Q_OBJECT

public:
    APLDB();

    void createAPLDB();
    bool checkMainTable(quint8 id);
    void addToMainTable();
};

#endif // APLDB_H
