#include <QFile>
#include "APLRead.h"

APL_LOGGING_CATEGORY(APLREAD_LOG,        "APLReadLog")

APLRead *APLRead::_instance = NULL;

APLRead::APLRead()
{
    _instance = this;
}

void APLRead::getFileDir(const QString &file_dir)
{
    getDatastream(file_dir);
}

void APLRead::getDatastream(const QString &file_dir)
{
    QFile file;

    file.setFileName(file_dir);
    if(!file.open(QIODevice::ReadOnly))
    {
         qCDebug(APLREAD_LOG)<< "can not open file!";
         return;
    }

    qCDebug(APLREAD_LOG)<< "open file OK";

    file.close();
}
