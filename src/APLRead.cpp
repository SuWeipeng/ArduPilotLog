#include "APLRead.h"

APL_LOGGING_CATEGORY(APLREAD_LOG,        "APLReadLog")

APLRead *APLRead::_instance = NULL;

APLRead::APLRead()
{
    _instance = this;
}

void APLRead::getFileDir(const QString &file)
{
    qCDebug(APLREAD_LOG) << file;
}
