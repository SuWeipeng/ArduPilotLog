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

    void addToSubTable(QString values);

    QString getFormat(quint8 &id);

private:
    QSqlDatabase _apldb;

    //true: create sub-table success
    bool _createSubTable(QString &name, QString &format, QString &field) const;

    void _createTableField(QString &format, QString &field, QString &table_field) const;
};

#endif // APLDB_H
