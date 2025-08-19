#include "DialogLoad.h"
#include "APLReadConf.h"
#include "mainwindow.h"
#include "APLRead.h"
#include <QFileDialog>

APL_LOGGING_CATEGORY(DialogLoad_LOG,        "DialogLoadLog")

DialogLoad::DialogLoad(QWidget *parent)
    : QDialog(parent)
    , _aplReadConf(new APLReadConf)
    , _qfileDialogLoad(new QFileDialog)
{
    connect(_qfileDialogLoad,  &QFileDialog::fileSelected, _aplReadConf, &APLReadConf::getFileDir);
}

DialogLoad::~DialogLoad()
{
    delete _aplReadConf;
    delete _qfileDialogLoad;
}

bool DialogLoad::isDirExist(QString fullPath)
{
    QDir dir(fullPath);

    if(dir.exists())
    {
        return true;
    }
    return false;
}

void DialogLoad::showFile()
{
    QString path =APLRead::getAPLRead()->getFilePath();

    if (isDirExist(QString("%1/conf").arg(path)))
    {
        path = QString("%1/conf").arg(path);
    } else {
        QString confdir_loc = "";
        QFile confdir(QString("confdir.txt"));
        if(!confdir.exists()){
            qCDebug(DialogLoad_LOG) << "confdir.txt doesn't exist!";
        } else {
            if(!confdir.open(QIODevice::ReadOnly | QIODevice::Text)){
                qCDebug(DialogLoad_LOG) << "opendir.txt read error!";
            } else {
                QTextStream pos(&confdir);
                confdir_loc = pos.readLine();
                qCDebug(DialogLoad_LOG) << confdir_loc;
            }
            confdir.close();
        }
        if(isDirExist(confdir_loc)){
            path = confdir_loc;
        }
    }

    QString confdir = _qfileDialogLoad->getOpenFileName(this
                                                  ,"open plot config"
                                                  ,path
                                                  ,"Config files(*.conf)");
    emit _qfileDialogLoad->fileSelected(confdir);

    qCDebug(DialogLoad_LOG) << confdir;
}

void DialogLoad::saveFile()
{

}
