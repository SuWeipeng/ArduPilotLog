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
    QFile::remove(DB_FILE);
}

void APLDB::createAPLDB()
{
    _apldb = QSqlDatabase::addDatabase("QSQLITE");
    _apldb.setDatabaseName(DB_FILE);

    if(!_apldb.open()){
        qCDebug(APLDB_LOG) << _apldb.lastError();
        qCDebug(APLDB_LOG) << _apldb.drivers();
    }else{
        QSqlQuery query;
        bool success = query.exec("CREATE TABLE maintable(id INT8 PRIMARY KEY, len INT8, name VARCHAR, format VARCHAR, labels VARCHAR)");
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
    _maintable_item.append(QString("INSERT INTO maintable VALUES(%1,%2,\"%3\",\"%4\",\"%5\")")
                               .arg(type).arg(len).arg(name,format,labels));
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
    query_insert.prepare(QString("INSERT INTO %1 VALUES(%2)").arg(name).arg(values));
    if(!query_insert.exec()){
        QSqlError queryErr = query_insert.lastError();
        //qCDebug(APLDB_LOG)<<"addToSubTable"<<queryErr.text();
    }
}

void APLDB::addToSubTableBuf(QString name, QString values)
{
    _name.append(name);
    _values.append(values);
}

void APLDB::buf2DB()
{
    QSqlQuery query_insert;
    for(quint64 i = 0; i<_maintable_item.length(); i++){
        if(!query_insert.exec(_maintable_item[i])){
            QSqlError queryErr = query_insert.lastError();
            qCDebug(APLDB_LOG)<<"buf2DB()"<<queryErr.text();
        }
    }
    _maintable_names.clear();
    _maintable_formats.clear();
    _maintable_ids.clear();
    _maintable_item.clear();

    for(quint64 i = 0; i<_name.length(); i++) {
        QString values = QString("%1,%2").arg(++_Number).arg(_values[i]);
        if(!query_insert.exec(QString("INSERT INTO %1 VALUES(%2)").arg(_name[i], values))){
            QSqlError queryErr = query_insert.lastError();
        }
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
    table_field = QString("%1,%2").arg("LineNo INTEGER PRIMARY KEY").arg(table_field);
    success = query_create.exec(QString("CREATE TABLE IF NOT EXISTS %1(%2)").arg(name).arg(table_field));

    if(!success){
        qCDebug(APLDB_LOG) << QString("CREATE TABLE IF NOT EXISTS %1(%2)").arg(name).arg(table_field);
    }

    return success;
}

void APLDB::_createTableField(QString &format, QString &field, QString &table_field) const
{
    QByteArray formatArray = format.toLatin1();

    if(formatArray.count() != field.count(',')+1){
        qCDebug(APLDB_LOG)<<"format and labels don't match";
        return;
    }

    for(qint8 i = 0; i< formatArray.count(); i++){
        switch(formatArray[i]){
        case 'a': //int16_t[32]
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("INTEGER");
            break;
        case 'b': //int8_t
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("INTEGER");
            break;
        case 'B': //uint8_t
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("INTEGER");
            break;
        case 'h': //int16_t
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("INTEGER");
            break;
        case 'H': //uint16_t
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("INTEGER");
            break;
        case 'i': //int32_t
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("INTEGER");
            break;
        case 'I': //uint32_t
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("INTEGER");
            break;
        case 'f': //float
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("DOUBLE");
            break;
        case 'd': //double
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("DOUBLE");
            break;
        case 'n': //char[4]
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("VARCHAR");
            break;
        case 'N': //char[16]
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("VARCHAR");
            break;
        case 'Z': //char[64]
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("VARCHAR");
            break;
        case 'c': //int16_t * 100
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("DOUBLE");
            break;
        case 'C': //uint16_t * 100
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("DOUBLE");
            break;
        case 'e': //int32_t * 100
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("DOUBLE");
            break;
        case 'E': //uint32_t * 100
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("DOUBLE");
            break;
        case 'L': //int32_t latitude/longitude
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("INTEGER");
            break;
        case 'M': //uint8_t flight mode
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("INTEGER");
            break;
        case 'q': //int64_t
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("INTEGER");
            break;
        case 'Q': //uint64_t
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("INTEGER");
            break;
        }
    }
    table_field.chop(1);
}

void APLDB::getFormat(quint8 &id, QString &name, QString &format)
{
    int idx = _maintable_ids.indexOf(QString("%1").arg(id));
    name = _maintable_names[idx];
    format = _maintable_formats[idx];
}

void APLDB::closeConnection()
{
    QString connection;
    connection = _apldb.connectionName();
    _apldb.close();
    _apldb = QSqlDatabase();
    _apldb.removeDatabase(connection);
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
    query.prepare(QString(" PRAGMA table_info('%1')").arg(table));
    if(query.exec()){
        query.last();
        return query.value(0).toInt();
    }

    return 0;
}

QString APLDB::getItemName(QSqlDatabase &db, QString table, int i)
{
    QSqlQuery query(db);
    query.prepare(QString(" PRAGMA table_info('%1')").arg(table));
    if(query.exec()){
        query.next();
        for(int n = 0; n < i; n++)
            query.next();
        return query.value(1).toString();
    }

    return "";
}

QString APLDB::getDiff(QSqlDatabase &db, QString table, QString field)
{
    QSqlQuery query(db);
    QByteArray ret;

    query.prepare(QString("SELECT DISTINCT %1 FROM %2").arg(field,table));
    if(query.exec()){
        while(query.next()){
            ret.append(query.value(0).toString().toUtf8());
            ret.append(",");
        }

        QString ret_str = QString(ret);
        return ret_str.left(ret_str.length() - 1);
    }

    return "";
}

int APLDB::getLen(QSqlDatabase &db, QString table, QString field)
{
    QSqlQuery query(db);
    query.prepare(QString("SELECT COUNT(%1) FROM %2").arg(field).arg(table));

    if(!query.exec()){
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

    query.prepare(QString("SELECT %1 FROM %2").arg(field).arg(table));

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

    query.prepare(QString("SELECT %1 FROM %2").arg(field).arg(table));

    if(!query.exec()){
        QSqlError queryErr = query.lastError();
        qCDebug(APLDB_LOG)<<queryErr.text();
    }

    query.seek(index);

    data = query.value(0).toDouble();
}

void APLDB::copy_table(QSqlDatabase &db, QString new_name, QString i, int i_value, QString fields, QString origin_name)
{
    QSqlQuery query(db);

    query.prepare(QString("CREATE TABLE %1 AS SELECT %2 FROM %3 WHERE %4=%5").arg(new_name).arg(fields).arg(origin_name).arg(i).arg(i_value));

    if(!query.exec()){
        QSqlError queryErr = query.lastError();
    }
}

void APLDB::deleteDataBase()
{
    if(isOpen()){
        close();
    }
    QFile::remove(DB_FILE);
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

    query.prepare(QString("SELECT COUNT(*) FROM %1").arg(table));

    if(!query.exec()){
        QSqlError queryErr = query.lastError();
        qCDebug(APLDB_LOG)<<queryErr.text();
    }
    query.next();

    if(query.value(0).toInt() == 0) return true;

    return false;
}

void APLDB::connectSQLite(QSqlDatabase &db)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DB_FILE);
    db.open();
}
