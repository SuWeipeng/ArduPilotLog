#include <QSqlError>
#include "APLDB.h"

APL_LOGGING_CATEGORY(APLDB_LOG,        "APLDBLog")

APLDB::APLDB()
{

}

void APLDB::createAPLDB()
{
    _apldb.addDatabase("QSQLITE");
    _apldb.setDatabaseName("APLDB.db");
    if(!_apldb.open()){
        qCDebug(APLDB_LOG) << _apldb.lastError();
        qCDebug(APLDB_LOG) << _apldb.drivers();
    }

}

bool APLDB::checkMainTable(quint8 id)
{
    qCDebug(APLDB_LOG) << "check main table "<<id;
    return false;
}

void APLDB::addToMainTable()
{
    qCDebug(APLDB_LOG) << "add to main table";
}
