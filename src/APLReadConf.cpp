#include <QFile>
#include <QDataStream>
#include <QRegExpValidator>
#include <QFileInfo>
#include "APLReadConf.h"
#include "mainwindow.h"

#define APPEND_STR ".0.0"

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
    QFile  file;

    file.setFileName(file_dir);
    if(!file.open(QIODevice::ReadOnly))
    {
         qCDebug(APLREAD_CONF_LOG)<< "can not open file!";
         return;
    }

    QTextStream in(&file);
    _decode(in);

    emit fileOpened();
    qCDebug(APLREAD_CONF_LOG) << "All plot config have been read";

    file.close();
}

void APLReadConf::_decode(QTextStream &in) const
{
    QStringList conf;
    QString lineStr;
    while(!in.atEnd())
    {
        lineStr = in.readLine();

        lineStr.append(APPEND_STR);
        qDebug()<<lineStr;

        if(lineStr.left(1).compare("#") == 0) continue;

        if(lineStr.compare(APPEND_STR) != 0){
            conf.append(lineStr);
        }
    }

    MainWindow::getMainWindow()->set_conf(conf);
}
