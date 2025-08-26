#ifndef APLDB_H
#define APLDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QVariant>
#include <QVector>
#include "APLLoggingCategory.h"

Q_DECLARE_LOGGING_CATEGORY(APLDB_LOG)

class APLDB : public QObject
{
    Q_OBJECT

public:
    APLDB();
    ~APLDB();

    void createAPLDB(const QString &dbPath);

    //true: id already exist
    bool checkMainTable(quint8 id);

    void addToMainTable(quint8 type,
                        quint8 len,
                        QString name,
                        QString format,
                        QString labels);

    void addToSubTable(QString name, QString values);

    void addToSubTableBuf(QString name, QVector<QVariant> values);

    void buf2DB();

    void insertBatchData(const QString& tableName, const QList<QVector<QVariant>>& rows);

    void getFormat(quint8 &id, QString &name, QString &format);

    bool isOpen() const { return _apldb.isOpen(); }

    void close()  { _apldb.close(); }

    void commit() { _apldb.commit(); }

    void transaction() { _apldb.transaction(); }

    void closeConnection();

    static QString getGroupName(QSqlDatabase &db, int i);

    static QString getTableName(QSqlDatabase &db, int i);

    static QString getItemName(QSqlDatabase &db, QString table, int i);

    static QString getDiff(QSqlDatabase &db, QString table, QString field);

    static int getGroupCount(QSqlDatabase &db);

    static int getTableNum(QSqlDatabase &db);

    static int getItemCount(QSqlDatabase &db, QString table);

    static int getLen(QSqlDatabase &db, QString table, QString field);

    static bool getData(QSqlDatabase &db, QString table, QString field, int len, QVector<double>& data, double offset = 0, double scale = 1);

    static void getData(QSqlDatabase &db, QString table, QString field, int index, double& data);

    static void copy_table(QSqlDatabase &db, QString new_name, QString i, int i_value, QString fields, QString origin_name);

    void deleteDataBase(const QString &dbdir);

    void reset();

    static bool isEmpty(QSqlDatabase &db, QString table);

private:
    QSqlDatabase  _apldb;
    quint32       _Number;
    QStringList   _name;
    QList<QVector<QVariant>>   _values;
    QStringList   _maintable_item;
    QStringList   _maintable_ids;
    QStringList   _maintable_names;
    QStringList   _maintable_formats;

    //true: create sub-table success
    bool _createSubTable(QString &name, QString &format, QString &field) const;

    void _createTableField(const QString &name, QString &format, QString &field, QString &table_field) const;

    QString _sanitizeFieldName(const QString &name, const QString& fieldName) const;
};

#endif // APLDB_H
