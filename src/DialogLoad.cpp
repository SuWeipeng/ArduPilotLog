#include "DialogLoad.h"
#include "APLReadConf.h"
#include "mainwindow.h"
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

void DialogLoad::showFile()
{
    QString confdir = _qfileDialogLoad->getOpenFileName(this
                                                  ,"open plot config"
                                                  ,"/"
                                                  ,"Binary files(*.conf)");
    emit _qfileDialogLoad->fileSelected(confdir);

    qCDebug(DialogLoad_LOG) << confdir;
}

void DialogLoad::saveFile()
{

}
