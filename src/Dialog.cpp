#include "Dialog.h"
#include "mainwindow.h"
#include "APLRead.h"
#include "APLDB.h"
#include <QFileDialog>
#include "APLDataCache.h"
#include <QFileInfo>
#include <QStringList>
#include <QMap>
#include <QFile>
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

class CSVProcessor
{
public:
    CSVProcessor(QString& dir): _dir(dir) {}

    // 获取CSV文件的表头
    QStringList getCSVHeaders(const QString &filePath)
    {
        QStringList headers;
        QFile file(filePath);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "无法打开文件:" << filePath;
            return headers;
        }

        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8); // 设置编码为UTF-8

        if (!in.atEnd()) {
            QString firstLine = in.readLine();
            // 按逗号分割，处理可能的引号包围的字段
            headers = parseCSVLine(firstLine);
        }

        file.close();
        return headers;
    }

    // 解析CSV行，处理引号包围的字段
    QStringList parseCSVLine(const QString &line)
    {
        QStringList result;
        QString current;
        bool inQuotes = false;

        for (int i = 0; i < line.length(); ++i) {
            QChar c = line[i];

            if (c == '"') {
                inQuotes = !inQuotes;
            } else if (c == ',' && !inQuotes) {
                result.append(current.trimmed());
                current.clear();
            } else {
                current.append(c);
            }
        }

        // 添加最后一个字段
        result.append(current.trimmed());

        // 移除字段两端的引号
        for (QString &field : result) {
            if (field.startsWith('"') && field.endsWith('"')) {
                field = field.mid(1, field.length() - 2);
            }
        }

        return result;
    }

    // 获取当前目录下所有CSV文件及其表头
    QMap<QString, QStringList> getAllCSVFilesWithHeaders()
    {
        QMap<QString, QStringList> csvFilesMap;

        // 获取当前目录
        QDir currentDir = _dir;

        // 设置文件过滤器，只获取CSV文件
        QStringList filters;
        filters << "*.csv" << "*.CSV";
        currentDir.setNameFilters(filters);

        // 获取所有CSV文件
        QFileInfoList fileList = currentDir.entryInfoList(QDir::Files);

        qDebug() << "找到" << fileList.size() << "个CSV文件";

        // 遍历每个CSV文件
        for (const QFileInfo &fileInfo : fileList) {
            QString fileName = fileInfo.fileName();
            QString filePath = fileInfo.absoluteFilePath();

            // 获取文件表头
            QStringList headers = getCSVHeaders(filePath);

            // 将文件名和表头添加到映射中
            csvFilesMap[fileName] = headers;
        }

        _csvFiles = csvFilesMap;

        return csvFilesMap;
    }

    // 打印所有CSV文件信息
    void printAllCSVInfo()
    {
        qDebug() << "\n========== CSV文件汇总 ==========";

        if (_csvFiles.isEmpty()) {
            qDebug() << "当前目录下没有找到CSV文件";
            return;
        }

        for (auto it = _csvFiles.begin(); it != _csvFiles.end(); ++it) {
            qDebug() << "\n文件名:" << it.key();
            qDebug() << "表头:";

            const QStringList &headers = it.value();
            for (int i = 0; i < headers.size(); ++i) {
                qDebug() << QString("  [%1] %2").arg(i + 1).arg(headers[i]);
            }
        }
    }

    // 获取指定文件名、指定列名的数据
    QStringList getColumnData(const QString &fileName, const QString &columnName)
    {
        QStringList columnData;

        // 构建完整文件路径
        QDir currentDir(_dir);
        QString filePath = currentDir.absoluteFilePath(fileName);

        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "无法打开文件:" << filePath;
            return columnData;
        }

        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);

        // 读取表头行
        if (in.atEnd()) {
            qWarning() << "文件为空:" << fileName;
            file.close();
            return columnData;
        }

        QString headerLine = in.readLine();
        QStringList headers = parseCSVLine(headerLine);

        // 查找目标列的索引
        int columnIndex = headers.indexOf(columnName);
        if (columnIndex == -1) {
            qWarning() << "在文件" << fileName << "中未找到列:" << columnName;
            file.close();
            return columnData;
        }

        // 读取数据行
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.trimmed().isEmpty()) {
                continue; // 跳过空行
            }

            QStringList fields = parseCSVLine(line);

            // 确保该行有足够的字段
            if (columnIndex < fields.size()) {
                columnData.append(fields[columnIndex]);
            } else {
                columnData.append(""); // 如果字段不足，添加空字符串
            }
        }

        file.close();
        return columnData;
    }

    // 获取指定文件名、多个列名的数据
    QMap<QString, QStringList> getMultipleColumnsData(const QString &fileName, const QStringList &columnNames)
    {
        QMap<QString, QStringList> result;

        // 初始化结果映射
        for (const QString &columnName : columnNames) {
            result[columnName] = QStringList();
        }

        // 构建完整文件路径
        QDir currentDir(_dir);
        QString filePath = currentDir.absoluteFilePath(fileName);

        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "无法打开文件:" << filePath;
            return result;
        }

        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);

        // 读取表头行
        if (in.atEnd()) {
            qWarning() << "文件为空:" << fileName;
            file.close();
            return result;
        }

        QString headerLine = in.readLine();
        QStringList headers = parseCSVLine(headerLine);

        // 查找目标列的索引
        QMap<QString, int> columnIndexes;
        for (const QString &columnName : columnNames) {
            int index = headers.indexOf(columnName);
            if (index != -1) {
                columnIndexes[columnName] = index;
            } else {
                qWarning() << "在文件" << fileName << "中未找到列:" << columnName;
            }
        }

        // 读取数据行
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.trimmed().isEmpty()) {
                continue; // 跳过空行
            }

            QStringList fields = parseCSVLine(line);

            // 为每个请求的列提取数据
            for (auto it = columnIndexes.begin(); it != columnIndexes.end(); ++it) {
                const QString &columnName = it.key();
                int columnIndex = it.value();

                if (columnIndex < fields.size()) {
                    result[columnName].append(fields[columnIndex]);
                } else {
                    result[columnName].append(""); // 如果字段不足，添加空字符串
                }
            }
        }

        file.close();
        return result;
    }

    // 获取指定文件的所有数据（以二维列表形式返回）
    QList<QStringList> getAllData(const QString &fileName, bool includeHeaders = false)
    {
        QList<QStringList> allData;

        // 构建完整文件路径
        QDir currentDir(_dir);
        QString filePath = currentDir.absoluteFilePath(fileName);

        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "无法打开文件:" << filePath;
            return allData;
        }

        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);

        bool isFirstLine = true;
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.trimmed().isEmpty()) {
                continue; // 跳过空行
            }

            QStringList fields = parseCSVLine(line);

            // 根据参数决定是否包含表头
            if (isFirstLine && !includeHeaders) {
                isFirstLine = false;
                continue;
            }

            allData.append(fields);
            isFirstLine = false;
        }

        file.close();
        return allData;
    }

    // 打印指定列的数据
    void printColumnData(const QString &fileName, const QString &columnName)
    {
        QStringList data = getColumnData(fileName, columnName);

        qDebug() << "\n========== 列数据 ==========";
        qDebug() << "文件名:" << fileName;
        qDebug() << "列名:" << columnName;
        qDebug() << "数据行数:" << data.size();
        qDebug() << "数据内容:";

        for (int i = 0; i < data.size(); ++i) {
            qDebug() << QString("  [%1] %2").arg(i + 1).arg(data[i]);
        }
    }
private:
    QString _dir;
    QMap<QString, QStringList> _csvFiles;
};

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
    "python_path": null,
    "python_ingnore_db": false
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

    if (jsonObj.contains("python_ingnore_db") && jsonObj["python_ingnore_db"].isBool()) {
        _python_ingnore_db = jsonObj["python_ingnore_db"].toBool();
        qCDebug(DIALOG_LOG) << "python_ingnore_db from settings.json:" << _python_ingnore_db;
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
                                                  ,"Binary files(*.bin *.BIN *.csv *.CSV)"
                                                  ,nullptr
                                                  ,QFileDialog::DontUseNativeDialog);
    _logdir = logdir;
    loadSettings();

    QFileInfo fileInfo(_logdir);
    QString suffix = fileInfo.suffix().toLower();
    _csv_mode = false;
    _csvFilesMap.clear();
    MainWindow::getMainWindow()->ui().actionExportCSV->setVisible(true);
    MainWindow::getMainWindow()->ui().actionSaveDBFile->setVisible(true);
    MainWindow::getMainWindow()->ui().actionTrim->setVisible(true);
    QList<QAction*> actions = MainWindow::getMainWindow()->ui().menuTools->actions();
    actions.at(2)->setVisible(true);
    if (suffix.compare("csv", Qt::CaseInsensitive) == 0) {
        _csv_mode = true;
        MainWindow::getMainWindow()->ui().actionExportCSV->setVisible(false);
        MainWindow::getMainWindow()->ui().actionSaveDBFile->setVisible(false);
        MainWindow::getMainWindow()->ui().actionTrim->setVisible(false);
        actions.at(2)->setVisible(false);
        QString baseName = fileInfo.completeBaseName();
        QString dirPath = fileInfo.absolutePath();

        qCDebug(DIALOG_LOG) << baseName << dirPath << fileInfo.dir().dirName();;

        CSVProcessor processor(dirPath);

        // 获取所有CSV文件和表头的映射
        _csvFilesMap = processor.getAllCSVFilesWithHeaders();

        emit gotCSVDir();

    } else if (suffix.compare("bin", Qt::CaseInsensitive) == 0) {
        _db_name = "";
        emit _qfiledialog->fileSelected(_logdir);
        qCDebug(DIALOG_LOG) << _logdir;
    }
}

QStringList Dialog::got_csvFieldData(const QString& fileName, const QString& fieldName) const
{
    QFileInfo fileInfo(_logdir);
    QString dirPath = fileInfo.absolutePath();
    CSVProcessor processor(dirPath);
    return processor.getColumnData(fileName, fieldName);
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

void Dialog::ignore_db(bool checked)
{
    QFile file(QString("settings.json"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    QString out_line("");
    while (!in.atEnd()) {
        QString line = in.readLine();

        if (line.indexOf("python_ingnore_db") != -1) {
            QStringList list = line.split(":");
            line = list[0]+": "+QString("%1").arg(checked ? "true" : "false");
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
}
