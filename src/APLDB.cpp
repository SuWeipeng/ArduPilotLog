#include "APLDB.h"

APL_LOGGING_CATEGORY(APLDB_LOG,        "APLDBLog")

APLDB::APLDB()
{

}

void APLDB::createAPLDB()
{
    qCDebug(APLDB_LOG) << "Create DB";
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
