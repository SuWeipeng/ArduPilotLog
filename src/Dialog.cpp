#include "Dialog.h"
#include "APLRead.h"
#include "APLDB.h"
#include "mainwindow.h"
#include <QFileDialog>

APL_LOGGING_CATEGORY(DIALOG_LOG,        "DialogLog")

SaveAsWorker::SaveAsWorker(QObject *parent)
    : QObject(parent)
{
}

SaveAsWorker::~SaveAsWorker()
{
}

void SaveAsWorker::run(const QString &dbdir)
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
        }
    }
    emit saveAsDone();
}

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , _aplRead(new APLRead)
    , _qfiledialog(new QFileDialog)
{
    _worker       = new SaveAsWorker();
    _workThread   = new QThread(this);
    _worker->moveToThread(_workThread);

    connect(_qfiledialog,  &QFileDialog::fileSelected, _aplRead, &APLRead::getFileDir);
    connect(this, &Dialog::saveAsStart, _worker, &SaveAsWorker::run);
    connect(_worker, &SaveAsWorker::saveAsDone, this, &Dialog::saveAsDone);

    _workThread->start();
}

Dialog::~Dialog()
{
    _workThread->quit();
    _workThread->wait();

    delete _workThread;
    delete _aplRead;
    delete _qfiledialog;
}

bool Dialog::isDirExist(QString fullPath)
{
    QDir dir(fullPath);

    if(dir.exists())
    {
        return true;
    }
    return false;
}

void Dialog::showFile()
{
    QString opendir_loc = "";
    QFile opendir(QString("opendir.txt"));
    if(!opendir.exists()){
        qCDebug(DIALOG_LOG) << "opendir.txt doesn't exist!";
    } else {
        if(!opendir.open(QIODevice::ReadOnly | QIODevice::Text)){
            qCDebug(DIALOG_LOG) << "opendir.txt read error!";
        } else {
            QTextStream pos(&opendir);
            opendir_loc = pos.readLine();
            qCDebug(DIALOG_LOG) << opendir_loc;
        }
        opendir.close();
    }

    QString log_dir = QString("%1").arg(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    if(isDirExist(opendir_loc)){
        log_dir = opendir_loc;
    }

    QString logdir = _qfiledialog->getOpenFileName(this
                                                  ,"open ArduPilot binary log file"
                                                  ,log_dir
                                                  ,"Binary files(*.bin *.BIN)"
                                                  ,nullptr
                                                  ,QFileDialog::DontUseNativeDialog);
    emit _qfiledialog->fileSelected(logdir);

    qCDebug(DIALOG_LOG) << logdir;
}

void Dialog::saveFile()
{
    QString dbdir = _qfiledialog->getSaveFileName(this
                                                  ,"Save as DB file"
                                                  ,QString("%1/%2").arg(_aplRead->getFilePath(), _aplRead->getFileName().section('.',0,0)+".db")
                                                  ,"DB files(*.db)");

    if (!dbdir.isNull())
    {
        emit saveAsStart(dbdir);
    }
}

void Dialog::saveAsDone()
{
    emit saveSuccess();
}
