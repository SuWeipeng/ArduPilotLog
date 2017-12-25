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
        bool success = query.exec("create table maintable(id int primary key, len int, name varchar, format varchar, labels varchar)");
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
    QSqlQuery query;
    query.exec(QString("INSERT INTO maintable VALUES(%1,%2,\"%3\",\"%4\",\"%5\")")
               .arg(type).arg(len).arg(name,format,labels));
}
