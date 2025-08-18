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
    ~APLDB();

    void createAPLDB();

    //true: id already exist
    bool checkMainTable(quint8 id);

    void addToMainTable(quint8 type,
                        quint8 len,
                        QString name,
                        QString format,
                        QString labels);

    void addToSubTable(QString name, QString values);

    void getFormat(quint8 &id, QString &name, QString &format);

    bool isOpen() const { return _apldb.isOpen(); }

    void close()  { _apldb.close(); }

    void commit() { _apldb.commit(); }

    void closeConnection();

    static QString getGroupName(QSqlDatabase &db, int i);

    static QString getItemName(QSqlDatabase &db, QString table, int i);

    static int getGroupCount(QSqlDatabase &db);

    static int getItemCount(QSqlDatabase &db, QString table);

    static int getLen(QSqlDatabase &db, QString table, QString field);

    static bool getData(QSqlDatabase &db, QString table, QString field, int len, QVector<double>& data, double offset = 0, double scale = 1);

    static void getData(QSqlDatabase &db, QString table, QString field, int index, double& data);

    void deleteDataBase();

    void reset();

    static bool isEmpty(QSqlDatabase &db, QString table);

    static void connectSQLite(QSqlDatabase &db);

private:
    QSqlDatabase  _apldb;
    quint32       _Number;

    //true: create sub-table success
    bool _createSubTable(QString &name, QString &format, QString &field) const;

    void _createTableField(QString &format, QString &field, QString &table_field) const; 
};

#endif // APLDB_H
