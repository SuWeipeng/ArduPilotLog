#include <QSqlError>
#include <QSqlQuery>
#include "APLDB.h"

APL_LOGGING_CATEGORY(APLDB_LOG,        "APLDBLog")

APLDB::APLDB()
{

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
    QSqlQuery query;

    if(!_apldb.isOpen()){
        qCDebug(APLDB_LOG) << "APLDB.db is closed";
        return false;
    }

    query.exec(QString("SELECT id FROM maintable WHERE id LIKE %1").arg(id));
    if(!query.next()){
        qCDebug(APLDB_LOG) << QString("type: %1 don't exist").arg(id);
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
        _createSubTable(name, format, labels);
    }else{
        qCDebug(APLDB_LOG) << "Sub table already exist";
    }

}

bool APLDB::_createSubTable(QString &name, QString &format, QString &field) const
{
    QSqlQuery query_create;
    QString   table_field = "";
    bool      success     = false;

    _createTableField(format, field, table_field);
    success = query_create.exec(QString("CREATE TABLE %1(%2)").arg(name).arg(table_field));
    if(success){
      qCDebug(APLDB_LOG) << "create subtable success";
    }else{
      qCDebug(APLDB_LOG) << "create subtable failed";
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
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("INTEGER");
            break;
        case 'C': //uint16_t * 100
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("INTEGER");
            break;
        case 'e': //int32_t * 100
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("INTEGER");
            break;
        case 'E': //uint32_t * 100
            table_field = QString ("%1 %2 %3,").arg(table_field).arg(field.section(',', i, i)).arg("INTEGER");
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
