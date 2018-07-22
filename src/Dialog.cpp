#include "Dialog.h"
#include "APLRead.h"
#include "APLDB.h"
#include <QFileDialog>

APL_LOGGING_CATEGORY(DIALOG_LOG,        "DialogLog")

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , _aplRead(new APLRead)
    , _qfiledialog(new QFileDialog)
{
    connect(_qfiledialog,  &QFileDialog::fileSelected, _aplRead, &APLRead::getFileDir);
}

Dialog::~Dialog()
{
    delete _aplRead;
    delete _qfiledialog;
}

void Dialog::showFile()
{
    QString logdir = _qfiledialog->getOpenFileName(this
                                                  ,"open ArduPilot binary log file"
                                                  ,"/"
                                                  ,"Binary files(*.bin)");
    emit _qfiledialog->fileSelected(logdir);

    qCDebug(DIALOG_LOG) << logdir;
}

void Dialog::saveFile()
{
    QString dbdir = _qfiledialog->getSaveFileName(this
                                                  ,"Save as DB file"
                                                  ,_aplRead->getFileName().section('.',0,0)
                                                  ,"DB files(*.db)");

    if (!dbdir.isNull())
    {
        QFile  file_in, file_out;

        file_in.setFileName(QString(DB_FILE));
        file_out.setFileName(dbdir);
        if(!file_in.open(QIODevice::ReadOnly))
        {
             qCDebug(DIALOG_LOG)<< "can not open file: " << QString(DB_FILE);
             return;
        } else {
            if(file_out.open(QIODevice::ReadOnly)){
                file_out.close();
                QFile::remove(dbdir);
                qCDebug(DIALOG_LOG)<< "delete old file" << dbdir;
            }
            if(file_out.open(QIODevice::WriteOnly)){
                QDataStream in(&file_in);
                QDataStream out(&file_out);
                while(!in.atEnd())
                {
                    quint8  currentByte;

                    in  >> currentByte;
                    out << currentByte;
                }
                file_in.close();
                file_out.close();
                emit saveSuccess();
            }
        }
    }
}
