#ifndef APLDB_H
#define APLDB_H

#include <QObject>
#include <QSqlDatabase>
#include "APLLoggingCategory.h"

#define DB_FILE "APLDB.db"

Q_DECLARE_LOGGING_CATEGORY(APLDB_LOG)

class APLDB : public QObject
{
    Q_OBJECT

public:
    APLDB();

    void createAPLDB();

    //true: id already exist
    bool checkMainTable(quint8 id);

    void addToMainTable(quint8 type,
                        quint8 len,
                        QString name,
                        QString format,
                        QString labels);

private:
    QSqlDatabase _apldb;
};

#endif // APLDB_H
