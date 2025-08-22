#include "Dialog.h"
#include "APLRead.h"
#include "APLDB.h"
#include <QFileDialog>
#include "APLDataCache.h"
#include <QFileInfo>
#include <QDebug>
#include <QStandardPaths> // 包含 QStandardPaths

#include <QJsonDocument> // 为支持JSON添加
#include <QJsonObject>   // 为支持JSON添加
#include <QDir>          // 为 QDir 添加

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
    APLDB apldb;
    QFileInfo fileInfo(dbdir);

    // Ensure the directory exists
    QDir dir(fileInfo.absolutePath());
    if (!dir.exists()) {
        dir.mkpath(fileInfo.absolutePath());
    }

    apldb.closeConnection(); // Close any existing connection
    apldb.deleteDataBase(dbdir);  // Delete existing DB file if any
    apldb.createAPLDB(dbdir);      // Create new DB and maintable

    if (!apldb.isOpen()) {
        qCDebug(DIALOG_LOG) << "Failed to open database: " << dbdir;
        emit saveAsDone();
        return;
    }

    APLDataCache* dataCache = APLDataCache::get_singleton();
    if (!dataCache) {
        qCDebug(DIALOG_LOG) << "APLDataCache singleton not available.";
        emit saveAsDone();
        return;
    }

    // Iterate through all message types in APLDataCache
    for (auto it = dataCache->getStore().constBegin(); it != dataCache->getStore().constEnd(); ++it) {
        const QString &messageName = it.key();
        const MessageData &messageData = it.value();

        // Add format to maintable
        apldb.addToMainTable(messageData.type,
                            messageData.headers.size(), // len
                            messageName,
                            messageData.format,
                            messageData.labels);
    }

    // Now add the actual data
    // This part needs to be carefully implemented to convert binary data to QVariant
    // and then call apldb.addToSubTableBuf
    // For now, we'll just call buf2DB to process maintable and clear buffers.
    // The actual data conversion and insertion will be a separate step.
    apldb.buf2DB(); // Process maintable items and clear buffers

    // Now, insert the actual binary data
    for (auto it = dataCache->getBinaryStore().constBegin(); it != dataCache->getBinaryStore().constEnd(); ++it) {
        const QString &messageName = it.key();
        const QList<QByteArray> &binaryRows = it.value();

        // Get the format string for this message type
        // We need to retrieve the MessageData from APLDataCache's _store
        // to get the format string.
        if (!dataCache->getStore().contains(messageName)) {
            qCDebug(DIALOG_LOG) << "Format not found for message: " << messageName;
            continue;
        }
        const MessageData &messageData = dataCache->getStore()[messageName];
        const QString &format = messageData.format;

        for (const QByteArray &row : binaryRows) {
            QVector<QVariant> parsedData = dataCache->parseBinaryData(row, format);
            apldb.addToSubTableBuf(messageName, parsedData);
        }
    }
    apldb.buf2DB(); // Call again to process the buffered binary data

    apldb.close(); // Close the database connection

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

    loadSettings(); // 在构造函数中加载设置

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

void Dialog::loadSettings()
{
    QFile settingsFile(QString("settings.json"));
    if (!settingsFile.exists()) {
        qCDebug(DIALOG_LOG) << "settings.json doesn't exist!";
        return;
    }

    if (!settingsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCDebug(DIALOG_LOG) << "settings.json read error!";
        return;
    }

    QByteArray jsonData = settingsFile.readAll();
    settingsFile.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qCDebug(DIALOG_LOG) << "Failed to create JSON doc or it's not a JSON object.";
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    if (jsonObj.contains("opendir") && jsonObj["opendir"].isString()) {
        _opendir = jsonObj["opendir"].toString();
        qCDebug(DIALOG_LOG) << "opendir from setting.json:" << _opendir;
    }

    if (jsonObj.contains("table_split") && jsonObj["table_split"].isBool()) {
        _table_split = jsonObj["table_split"].toBool();
        qCDebug(DIALOG_LOG) << "table_split from settings.json:" << _table_split;
        _trim_from = jsonObj["trim_from"].toInteger();
        _trim_to   = jsonObj["trim_to"].toInteger();
        APLDataCache* cache = APLDataCache::get_singleton();
        if (cache) {
            cache->setTableSplit(_table_split);
            cache->setSaveCSV(jsonObj["save_csv"].toBool());
            cache->setTrimFrom(_trim_from);
            cache->setTrimTo(_trim_to);
        }
    }

    emit settingsLoaded(jsonObj);
}

void Dialog::showFile()
{
    QString log_dir = QString("%1").arg(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    if(isDirExist(_opendir)){
        log_dir = _opendir;
    }

    QString logdir = _qfiledialog->getOpenFileName(this
                                                  ,"open ArduPilot binary log file"
                                                  ,log_dir
                                                  ,"Binary files(*.bin *.BIN)"
                                                  ,nullptr
                                                  ,QFileDialog::DontUseNativeDialog);
    loadSettings();
    emit _qfiledialog->fileSelected(logdir);

    qCDebug(DIALOG_LOG) << logdir;
}

void Dialog::saveFile()
{
    QString suffix = ".db";
    if (_trim_from < _trim_to) {
        suffix = "_trim.db";
    }
    QString dbdir = _qfiledialog->getSaveFileName(this
                                                  ,"Save as DB file"
                                                  ,QString("%1/%2").arg(_aplRead->getFilePath(), _aplRead->getFileName().section('.',0,0)+suffix)
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
