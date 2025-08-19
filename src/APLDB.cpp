#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include "APLDB.h"

APL_LOGGING_CATEGORY(APLDB_LOG,        "APLDBLog")

APLDB::APLDB()
    : _Number(0)
{
    _name.clear();
    _values.clear();
    _maintable_names.clear();
    _maintable_formats.clear();
    _maintable_ids.clear();
    _maintable_item.clear();
}

APLDB::~APLDB()
{
    qCDebug(APLDB_LOG) << "APLDB::~APLDB()";
    if(isOpen()){
        close();
    }
}

void APLDB::createAPLDB(const QString &dbPath)
{
    _apldb = QSqlDatabase::addDatabase("QSQLITE");
    _apldb.setDatabaseName(dbPath);

    if(!_apldb.open()){
        qCDebug(APLDB_LOG) << _apldb.lastError();
        qCDebug(APLDB_LOG) << _apldb.drivers();
    }else{
        QSqlQuery query;
        bool success = query.exec("CREATE TABLE maintable(LineNo INTEGER PRIMARY KEY AUTOINCREMENT, id INT8, len INT8, name VARCHAR UNIQUE, format VARCHAR, labels VARCHAR, units VARCHAR, multipliers VARCHAR)");
        if(success){
          qCDebug(APLDB_LOG) << "create maintable success";
        }else{
          qCDebug(APLDB_LOG) << "create maintable failed";
        }
    }
}

//true: id already exist
bool APLDB::checkMainTable(quint8 id)
{
    bool ret = false;
    if(_maintable_ids.contains(QString("%1").arg(id))){
        ret = true;
    }

    return ret;
}

void APLDB::addToMainTable(quint8 type,
                           quint8 len,
                           QString name,
                           QString format,
                           QString labels)
{
    if (_maintable_names.contains(name)) {
        return;
    }

    _maintable_item.append(QString("INSERT INTO maintable(id, len, name, format, labels) VALUES(%1,%2,\"%3\",\"%4\",\"%5\")")
                               .arg(type)
                               .arg(len)
                               .arg(name)
                               .arg(format)
                               .arg(labels));

    _maintable_ids.append(QString("%1").arg(type));
    _maintable_names.append(name);
    _maintable_formats.append(format);

    if(!_createSubTable(name, format, labels)){
        qCDebug(APLDB_LOG) << name << "Sub table create fail";
    }
}

void APLDB::addToSubTable(QString name, QString values)
{
    QSqlQuery query_insert;

    values = QString("%1,%2").arg(++_Number).arg(values);
    query_insert.prepare(QString("INSERT INTO %1 VALUES(%2)").arg(name, values));
    if(!query_insert.exec()){
        QSqlError queryErr = query_insert.lastError();
        qCDebug(APLDB_LOG)<<"addToSubTable"<<queryErr.text();
    }
}

void APLDB::addToSubTableBuf(QString name, QVector<QVariant> values)
{
    _name.append(name);
    _values.append(values);
}

void APLDB::buf2DB()
{
    QSqlQuery query_insert;
    // Process maintable items
    for(qsizetype i = 0; i<_maintable_item.length(); i++){
        if(!query_insert.exec(_maintable_item[i])){
            QSqlError queryErr = query_insert.lastError();
            qCDebug(APLDB_LOG)<<"buf2DB()"<<queryErr.text();
        }
    }
    _maintable_names.clear();
    _maintable_formats.clear();
    _maintable_ids.clear();
    _maintable_item.clear();

    // Process subtable data using parameterized queries
    QMap<QString, QList<QVector<QVariant>>> grouped_data;
    for(qsizetype i = 0; i < _name.length(); ++i) {
        grouped_data[_name[i]].append(_values[i]);
    }

    for (auto it = grouped_data.constBegin(); it != grouped_data.constEnd(); ++it) {
        const QString &tableName = it.key();
        const QList<QVector<QVariant>> &rows = it.value();

        if (rows.isEmpty()) continue;

        QString placeholders = "?";
        for (int j = 0; j < rows.first().size(); ++j) {
            placeholders += ", ?";
        }

        QString insert_sql = QString("INSERT INTO \"%1\" VALUES(%2)").arg(tableName, placeholders);
        query_insert.prepare(insert_sql);

        _apldb.transaction();

        // 关键修改：为每个子表使用独立的行号计数器
        qint64 lineCounter = 0;

        for (const QVector<QVariant> &row_values : rows) {
            // 使用局部的、针对当前表的 lineCounter，而不是全局的 _Number
            query_insert.bindValue(0, ++lineCounter);
            for (int j = 0; j < row_values.size(); ++j) {
                query_insert.bindValue(j + 1, row_values[j]);
            }
            if(!query_insert.exec()){
                QSqlError queryErr = query_insert.lastError();
                qCDebug(APLDB_LOG)<<"buf2DB() - subtable"<<queryErr.text();
                _apldb.rollback();
                break;
            }
        }
        _apldb.commit();
    }

    _name.clear();
    _values.clear();
}

bool APLDB::_createSubTable(QString &name, QString &format, QString &field) const
{
    QSqlQuery query_create;
    QString   table_field = "";
    bool      success     = false;

    _createTableField(format, field, table_field);
    table_field = QString("%1,%2").arg("LineNo INTEGER PRIMARY KEY", table_field);
    success = query_create.exec(QString("CREATE TABLE IF NOT EXISTS \"%1\"(%2)").arg(name, table_field));

    if(!success){
        qCDebug(APLDB_LOG) << QString("CREATE TABLE IF NOT EXISTS \"%1\"(%2) FAILED: ").arg(name, table_field) << query_create.lastError().text();
    }

    return success;
}

void APLDB::_createTableField(QString &format, QString &field, QString &table_field) const
{
    QByteArray formatArray = format.toLatin1();
    QStringList fieldList = field.split(',');

    if(formatArray.count() != fieldList.count()){
        qCDebug(APLDB_LOG)<<"format and labels don't match";
        return;
    }

    for(qint8 i = 0; i< formatArray.count(); i++){
        QString fieldName = QString("\"%1\"").arg(fieldList[i]);
        switch(formatArray[i]){
        case 'a':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "INTEGER");
            break;
        case 'b':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "INTEGER");
            break;
        case 'B':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "INTEGER");
            break;
        case 'h':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "INTEGER");
            break;
        case 'H':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "INTEGER");
            break;
        case 'i':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "INTEGER");
            break;
        case 'I':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "INTEGER");
            break;
        case 'f':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "DOUBLE");
            break;
        case 'd':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "DOUBLE");
            break;
        case 'n':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "VARCHAR");
            break;
        case 'N':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "VARCHAR");
            break;
        case 'Z':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "VARCHAR");
            break;
        case 'c':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "DOUBLE");
            break;
        case 'C':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "DOUBLE");
            break;
        case 'e':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "DOUBLE");
            break;
        case 'E':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "DOUBLE");
            break;
        case 'L':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "INTEGER");
            break;
        case 'M':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "INTEGER");
            break;
        case 'q':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "INTEGER");
            break;
        case 'Q':
            table_field = QString ("%1 %2 %3,").arg(table_field, fieldName, "INTEGER");
            break;
        }
    }
    table_field.chop(1);
}

void APLDB::getFormat(quint8 &id, QString &name, QString &format)
{
    int idx = _maintable_ids.indexOf(QString("%1").arg(id));
    if (idx != -1) {
        name = _maintable_names[idx];
        format = _maintable_formats[idx];
    }
}

void APLDB::closeConnection()
{
    QString connection;
    connection = _apldb.connectionName();
    _apldb.close();
    _apldb = QSqlDatabase();
    QSqlDatabase::removeDatabase(connection);
}

QString APLDB::getGroupName(QSqlDatabase &db, int i)
{
    QSqlQuery query(db);

    query.exec(QString("SELECT name FROM maintable"));
    for(int n = 0; n < i; n++)
        query.next();

    return  query.value(0).toString();
}

QString APLDB::getTableName(QSqlDatabase &db, int i)
{
    QSqlQuery query(db);

    query.exec(QString("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name"));
    for(int n = 0; n < i; n++)
        query.next();

    return  query.value(0).toString();
}

int APLDB::getGroupCount(QSqlDatabase &db)
{
    QSqlQuery query(db);

    query.exec(QString("SELECT COUNT(name) FROM maintable"));
    query.next();

    return  query.value(0).toInt();
}

int APLDB::getTableNum(QSqlDatabase &db)
{
    QSqlQuery query(db);

    query.exec(QString("SELECT COUNT(*) FROM sqlite_master WHERE type='table'"));
    query.next();

    return  query.value(0).toInt();
}

int APLDB::getItemCount(QSqlDatabase &db, QString table)
{
    QSqlQuery query(db);

    query.prepare(QString("PRAGMA table_info(\"%1\")").arg(table));
    if(query.exec()){
        int count = 0;
        while(query.next()) {
            count++;
        }
        return count;
    }

    return 0;
}

QString APLDB::getItemName(QSqlDatabase &db, QString table, int i)
{
    QSqlQuery query(db);

    query.prepare(QString("PRAGMA table_info(\"%1\")").arg(table));
    if(query.exec()){
        for(int n = 0; n <= i; n++) {
            if (!query.next()) return "";
        }
        return query.value(1).toString();
    }

    return "";
}

QString APLDB::getDiff(QSqlDatabase &db, QString table, QString field)
{
    QSqlQuery query(db);
    QByteArray ret;

    query.prepare(QString("SELECT DISTINCT \"%1\" FROM \"%2\"").arg(field,table));
    if(query.exec()){
        while(query.next()){
            ret.append(query.value(0).toString().toUtf8());
            ret.append(",");
        }

        QString ret_str = QString(ret);
        if (!ret_str.isEmpty()) {
            ret_str.chop(1);
        }
        return ret_str;
    }

    return "";
}

int APLDB::getLen(QSqlDatabase &db, QString table, QString field)
{
    QSqlQuery query(db);

    query.prepare(QString("SELECT COUNT(\"%1\") FROM \"%2\"").arg(field).arg(table));

    if(!query.exec()){
        qCDebug(APLDB_LOG)<<"getLen(QSqlDatabase &db, QString table, QString field)";
        QSqlError queryErr = query.lastError();
        qCDebug(APLDB_LOG)<<queryErr.text();
        return 0;
    }

    query.next();

    return query.value(0).toInt();
}

bool APLDB::getData(QSqlDatabase &db, QString table, QString field, int len, QVector<double>& data, double offset, double scale)
{
    QSqlQuery query(db);

    query.prepare(QString("SELECT \"%1\" FROM \"%2\"").arg(field).arg(table));

    if(!query.exec()){
        QSqlError queryErr = query.lastError();
        qCDebug(APLDB_LOG)<<queryErr.text();
        return false;
    }

    for(int i=0; i<len; i++){
        query.next();
        data[i] = (query.value(0).toDouble() + offset) * scale;
    }
    return true;
}

void APLDB::getData(QSqlDatabase &db, QString table, QString field, int index, double& data)
{
    QSqlQuery query(db);

    query.prepare(QString("SELECT \"%1\" FROM \"%2\"").arg(field).arg(table));

    if(!query.exec()){
        QSqlError queryErr = query.lastError();
        qCDebug(APLDB_LOG)<<queryErr.text();
    }

    if(query.seek(index)){
        data = query.value(0).toDouble();
    }
}

void APLDB::copy_table(QSqlDatabase &db, QString new_name, QString i, int i_value, QString fields, QString origin_name)
{
    QSqlQuery query(db);

    query.prepare(QString("CREATE TABLE \"%1\" AS SELECT %2 FROM \"%3\" WHERE \"%4\"=%5").arg(new_name).arg(fields).arg(origin_name).arg(i).arg(i_value));

    if(!query.exec()){
        QSqlError queryErr = query.lastError();
    }
}

void APLDB::deleteDataBase(const QString &dbdir)
{
    if(isOpen()){
        close();
    }
    if (QFile::exists(dbdir)) {
        QFile::remove(dbdir);
    }
}

void APLDB::reset()
{
    _Number = 0;
    _name.clear();
    _values.clear();
    _maintable_names.clear();
    _maintable_formats.clear();
    _maintable_ids.clear();
    _maintable_item.clear();
}

bool APLDB::isEmpty(QSqlDatabase &db, QString table)
{
    QSqlQuery query(db);

    query.prepare(QString("SELECT COUNT(*) FROM \"%1\"").arg(table));

    if(!query.exec()){
        QSqlError queryErr = query.lastError();
        qCDebug(APLDB_LOG)<<queryErr.text();
    }
    query.next();

    if(query.value(0).toInt() == 0) return true;

    return false;
}
