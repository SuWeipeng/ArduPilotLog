#ifndef PYTHONEXPORTERCSV_H
#define PYTHONEXPORTERCSV_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDir>

struct CSVDataField {
    QString logType;     // 如 "TECS", "ARSP0", "GPS0", "BARO0"
    QStringList fields;  // 如 ["TimeUS", "th", "hin", "sp", ...]
};

class PythonExporterCSV
{
public:
    static PythonExporterCSV* get_singleton() {
        return _singleton;
    }

    PythonExporterCSV();

    // 添加数据字段
    void addDataField(const QString& logType, const QStringList& fields);

    // 设置日志文件夹名称
    void setLogFolderName(const QString& folderName);

    // 导出 Python 文件
    bool exportToPython(const QString& outputPath);

    // 清空所有数据字段
    void clear();

private:
    static PythonExporterCSV* _singleton;

    QString generatePythonCode();
    QString generateVariableNames(const QString& logType, const QString& field);
    QString generateDataReading(const CSVDataField& field);

    QList<CSVDataField> m_dataFields;
    QString m_logFolderName;
};

#endif // PYTHONEXPORTERCSV_H