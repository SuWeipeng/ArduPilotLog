#include "Dialog.h"
#include "mainwindow.h"
#include "APLRead.h"
#include "APLDB.h"
#include <QFileDialog>
#include "APLDataCache.h"
#include <QFileInfo>
#include <QDebug>
#include <QTextStream>
#include <QStandardPaths> // 包含 QStandardPaths

#include <QJsonDocument> // 为支持JSON添加
#include <QJsonObject>   // 为支持JSON添加
#include <QDir>          // 为 QDir 添加

APL_LOGGING_CATEGORY(DIALOG_LOG,        "DialogLog")

QString preprocessJsonData(const QByteArray& jsonData) {
    QString jsonString = QString::fromUtf8(jsonData);
    QString result;

    bool inString = false;
    bool inSingleLineComment = false;
    bool inMultiLineComment = false;
    bool escapeNext = false;

    for (int i = 0; i < jsonString.length(); ++i) {
        QChar current = jsonString[i];
        QChar next = (i + 1 < jsonString.length()) ? jsonString[i + 1] : QChar();

        if (escapeNext) {
            if (inString) result += current;
            escapeNext = false;
            continue;
        }

        if (current == '\\' && inString) {
            result += current;
            escapeNext = true;
            continue;
        }

        if (inSingleLineComment) {
            if (current == '\n') {
                inSingleLineComment = false;
                result += current;
            }
            continue;
        }

        if (inMultiLineComment) {
            if (current == '*' && next == '/') {
                inMultiLineComment = false;
                ++i; // 跳过 '/'
            }
            continue;
        }

        if (current == '"' && !inString) {
            inString = true;
            result += current;
        } else if (current == '"' && inString) {
            inString = false;
            result += current;
        } else if (!inString && current == '/' && next == '/') {
            inSingleLineComment = true;
            ++i; // 跳过第二个 '/'
        } else if (!inString && current == '/' && next == '*') {
            inMultiLineComment = true;
            ++i; // 跳过 '*'
        } else if (!inSingleLineComment && !inMultiLineComment) {
            result += current;
        }
    }

    return result;
}

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
        qCDebug(DIALOG_LOG) << "settings.json doesn't exist! Creating default settings file.";

        // 创建默认设置内容
        QString defaultSettingsJson = R"({
    "opendir": "D:/Log",
    "table_split": false,
    "save_csv": false,
    "trim_from": 0,
    "trim_to": 0,
    "filter_file": null,
    "python_path": null
})";

        // 打开文件进行写入
        if (settingsFile.open(QIODevice::WriteOnly)) {
            settingsFile.write(defaultSettingsJson.toUtf8());
            settingsFile.close();
            qCDebug(DIALOG_LOG) << "Default settings.json created successfully.";
        } else {
            qCDebug(DIALOG_LOG) << "Failed to create settings.json file.";
            return;
        }
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

    // --- Start of Filter Function Logic ---
    if (jsonObj.contains("filter_file") && jsonObj["filter_file"].isString()) {
        _filter_file = jsonObj["filter_file"].toString();
        qCDebug(DIALOG_LOG) << "filter_file from setting.json:" << _filter_file;

        QFile filterFile(_filter_file);
        if (!filterFile.exists()) {
            // 创建默认设置内容
            QString defaultJson = R"({
    "filter_mode": 0, // 0-Include, 1-Exclude
    "include": "ATT, BARO",
    "exclude": "ATT, BARO"
})";

            // 打开文件进行写入
            if (filterFile.open(QIODevice::WriteOnly)) {
                filterFile.write(defaultJson.toUtf8());
                filterFile.close();
                qCDebug(DIALOG_LOG) << "Default settings.json created successfully.";
            } else {
                qCDebug(DIALOG_LOG) << "Failed to create settings.json file.";
                return;
            }
        }
        else if (!filterFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qCDebug(DIALOG_LOG) << _filter_file << " read error!";
        }
        else
        {
            QByteArray jsonData = filterFile.readAll();
            filterFile.close();

            QString cleanJsonString = preprocessJsonData(jsonData);
            QJsonDocument jsonDoc = QJsonDocument::fromJson(cleanJsonString.toUtf8());
            if (jsonDoc.isNull() || !jsonDoc.isObject()) {
                qCDebug(DIALOG_LOG) << "Failed to create JSON doc or it's not a JSON object.";
                return;
            }

            QJsonObject jsonObj = jsonDoc.object();

            if (jsonObj.contains("filter_mode")) {
                _filter_mode = jsonObj["filter_mode"].toInt();
                qCDebug(DIALOG_LOG) << "filter_mode from" << _filter_file << " is: "<< ((_filter_mode==0) ? QString("Include") : QString("Exclude"));
                _filter_include = jsonObj["include"].toString().split(",");
                for (QString &str : _filter_include) {
                    str = str.trimmed();
                }
                _filter_exclude = jsonObj["exclude"].toString().split(",");
                for (QString &str : _filter_exclude) {
                    str = str.trimmed();
                }
                qCDebug(DIALOG_LOG) << "Include:" << _filter_include;
                qCDebug(DIALOG_LOG) << "Exclude:" << _filter_exclude;

                APLDataCache* cache = APLDataCache::get_singleton();
                if (cache) {
                    cache->setFilterMode(_filter_mode);
                    cache->setFilterInclude(_filter_include);
                    cache->setFilterExclude(_filter_exclude);
                    cache->setFilterFile(_filter_file);
                }
            }
        }
    } else {
        APLDataCache* cache = APLDataCache::get_singleton();
        if (cache) {
            cache->setFilterMode(-1);
        }
        qCDebug(DIALOG_LOG) << "filter_file is not configured.";
    }
    // --- End of Filter Function Logic ---

    // --- Start of Python Path Logic ---
    if (jsonObj.contains("python_path") && jsonObj["python_path"].isString()) {
        _python_path = jsonObj["python_path"].toString();
        qCDebug(DIALOG_LOG) << "python_path from setting.json:" << _python_path;

        QFile pythonPath(_python_path);
        if (!pythonPath.exists()) {
            _python_path = "";
            qCDebug(DIALOG_LOG) << "Invalid python_path.";
        }
    }
    // --- End of Python Path Logic ---

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

    _db_name = "";

    _logdir = logdir;

    emit _qfiledialog->fileSelected(_logdir);

    qCDebug(DIALOG_LOG) << _logdir;
}

void Dialog::saveFile()
{
    QString file_name = _aplRead->getFileName().section('.',0,0);
    QString suffix = ".db";
    if(_filter_file.length()>0) {
        file_name.append("_");
        file_name.append(_filter_file.section('.',0,0));
        file_name.append(QString("-%1").arg(_filter_mode));
    }
    if (_trim_from < _trim_to) {
        file_name.append("_trim");
    }
    file_name.append(suffix);
    QString dbdir = _qfiledialog->getSaveFileName(this
                                                  ,"Save as DB file"
                                                  ,QString("%1/%2").arg(_aplRead->getFilePath(), file_name)
                                                  ,"DB files(*.db)");
    _db_name = dbdir;
    if (!dbdir.isNull())
    {
        emit saveAsStart(dbdir);
    }
}

void Dialog::saveAsDone()
{
    emit saveSuccess();
}

void Dialog::trim()
{
    if (MainWindow::getMainWindow()->get_x_us().length() == 0) {
        _trim_from =0;
        _trim_to = 0;
        _db_name = "";
    }

    QFile file(QString("settings.json"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    QString out_line("");
    while (!in.atEnd()) {
        QString line = in.readLine();

        if (line.indexOf("trim_from") != -1) {
            QStringList list = line.split(":");
            line = list[0]+": "+QString("%1").arg(_trim_from)+",";
        }

        if (line.indexOf("trim_to") != -1) {
            QStringList list = line.split(":");
            line = list[0]+": "+QString("%1").arg(_trim_to)+",";
        }
        out_line.append(line+"\n");
    }
    file.close();

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    out << out_line;
    file.close();

    loadSettings();

    MainWindow::getMainWindow()->ui().treeWidget->clear();
    MainWindow::getMainWindow()->clearGraph();
    emit _qfiledialog->fileSelected(_logdir);
}

void Dialog::split(bool checked)
{
    QFile file(QString("settings.json"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    QString out_line("");
    while (!in.atEnd()) {
        QString line = in.readLine();

        if (line.indexOf("table_split") != -1) {
            QStringList list = line.split(":");
            line = list[0]+": "+QString("%1").arg(checked ? "true" : "false")+",";
        }

        out_line.append(line+"\n");
    }
    file.close();

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    out << out_line;
    file.close();

    loadSettings();

    MainWindow::getMainWindow()->ui().treeWidget->clear();
    MainWindow::getMainWindow()->clearGraph();
    emit _qfiledialog->fileSelected(_logdir);
}
