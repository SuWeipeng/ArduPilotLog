#include <QFile>
#include <QDataStream>
#include <QRegExpValidator>
#include <QFileInfo>
#include "APLReadConf.h"

APL_LOGGING_CATEGORY(APLREAD_CONF_LOG,        "APLReadConfLog")

APLReadConf::APLReadConf()
{
}

APLReadConf::~APLReadConf()
{
}

void APLReadConf::getFileDir(const QString &file_dir)
{
    _file_name = QFileInfo(file_dir).fileName();
    getDatastream(file_dir);

    qCDebug(APLREAD_CONF_LOG) << file_dir;
}

void APLReadConf::getDatastream(const QString &file_dir)
{

}
