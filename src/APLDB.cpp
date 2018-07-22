#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include "APLDB.h"

APL_LOGGING_CATEGORY(APLDB_LOG,        "APLDBLog")

APLDB *APLDB::_instance;

APLDB::APLDB()
    : _Number(0)
{
    _instance = this;
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
        _apldb.transaction();
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
    QSqlQuery query;

    if(!_apldb.isOpen()){
        qCDebug(APLDB_LOG) << QString(DB_FILE) << " is closed";
        return false;
    }

    query.exec(QString("SELECT id FROM maintable WHERE id LIKE %1").arg(id));
    if(!query.next()){
        //qCDebug(APLDB_LOG) << QString("type: %1 don't exist").arg(id);
        return false;
    }

    return true;
}

void APLDB::addToMainTable(quint8 type,
                           quint8 len,
                           QString name,
                           QString format,
                           QString labels)
{
    QSqlQuery query_insert;
    QSqlQuery query_check;

    query_insert.exec(QString("INSERT INTO maintable VALUES(%1,%2,\"%3\",\"%4\",\"%5\")")
                      .arg(type).arg(len).arg(name,format,labels));

    query_check.exec(QString("SELECT COUNT(*) FROM SQLITE_MASTER WHERE TYPE='table' AND name='%1'").arg(name));

    query_check.next();

    if(query_check.value(0).toInt()==0){
        if(!_createSubTable(name, format, labels)){
            qCDebug(APLDB_LOG) << name << "Sub table create fail";
        }
    }else{
        qCDebug(APLDB_LOG) << "Sub table already exist";
    }

}

void APLDB::addToSubTable(QString name, QString values)
{
    QSqlQuery query_insert;

    values = QString("%1,%2").arg(++_Number).arg(values);
    query_insert.prepare(QString("INSERT INTO %1 VALUES(%2)").arg(name).arg(values));
    if(!query_insert.exec()){
        QSqlError queryErr = query_insert.lastError();
        qCDebug(APLDB_LOG)<<"addToSubTable"<<queryErr.text();
    }
}

bool APLDB::_createSubTable(QString &name, QString &format, QString &field) const
{
    QSqlQuery query_create;
    QString   table_field = "";
    bool      success     = false;

    _createTableField(format, field, table_field);
    table_field = QString("%1,%2").arg("LineNo INTEGER PRIMARY KEY").arg(table_field);
    success = query_create.exec(QString("CREATE TABLE %1(%2)").arg(name).arg(table_field));

    if(!success){
        qCDebug(APLDB_LOG) << QString("CREATE TABLE %1(%2)").arg(name).arg(table_field);
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
    QSqlQuery query;

    query.exec(QString("SELECT id,name,format FROM maintable WHERE id = %1").arg(id));
    query.next();
    name   = query.value(1).toString();
    format = query.value(2).toString();
}

QString APLDB::getGroupName(int i)
{
    QSqlQuery query;

    query.exec(QString("SELECT name FROM maintable"));
    for(int n = 0; n < i; n++)
        query.next();

    return  query.value(0).toString();
}

int APLDB::getGroupCount()
{
    QSqlQuery query;

    query.exec(QString("SELECT COUNT(name) FROM maintable"));
    query.next();

    return  query.value(0).toInt();
}

int APLDB::getItemCount(QString table)
{
    QSqlQuery query;
    query.prepare(QString(" PRAGMA table_info('%1')").arg(table));
    if(query.exec()){
        query.last();
        return query.value(0).toInt();
    }

    return 0;
}

QString APLDB::getItemName(QString table, int i)
{
    QSqlQuery query;
    query.prepare(QString(" PRAGMA table_info('%1')").arg(table));
    if(query.exec()){
        query.next();
        for(int n = 0; n < i; n++)
            query.next();
        return query.value(1).toString();
    }

    return "";
}

int APLDB::getLen(QString table, QString field)
{
    QSqlQuery query;
    query.prepare(QString("SELECT COUNT(%1) FROM %2").arg(field).arg(table));

    if(!query.exec()){
        QSqlError queryErr = query.lastError();
        qCDebug(APLDB_LOG)<<queryErr.text();
        return 0;
    }

    query.next();

    return query.value(0).toInt();
}

bool APLDB::getData(QString table, QString field, int len, QVector<double>& data, double offset, double scale)
{
    QSqlQuery query;

    query.prepare(QString("SELECT %1 FROM %2").arg(field).arg(table));

    if(!query.exec()){
        QSqlError queryErr = query.lastError();
        qCDebug(APLDB_LOG)<<queryErr.text();
        return false;
    }

    for(int i=0; i<len; i++){
        query.next();
        data[i] = query.value(0).toDouble() * scale + offset;
    }
    return true;
}

void APLDB::getData(QString table, QString field, int index, double& data)
{
    QSqlQuery query;

    query.prepare(QString("SELECT %1 FROM %2").arg(field).arg(table));

    if(!query.exec()){
        QSqlError queryErr = query.lastError();
        qCDebug(APLDB_LOG)<<queryErr.text();
    }

    query.seek(index);

    data = query.value(0).toDouble();
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
}

bool APLDB::isEmpty(QString table)
{
    QSqlQuery query;

    query.prepare(QString("SELECT COUNT(*) FROM %1").arg(table));

    if(!query.exec()){
        QSqlError queryErr = query.lastError();
        qCDebug(APLDB_LOG)<<queryErr.text();
    }
    query.next();

    if(query.value(0).toInt() == 0) return true;

    return false;
}
