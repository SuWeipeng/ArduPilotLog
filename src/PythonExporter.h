#ifndef PYTHONEXPORTER_H
#define PYTHONEXPORTER_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDir>

struct DataField {
    QString logType;     // 如 "TECS"
    QStringList fields;  // 如 ["TimeUS", "th", "hin", "sp", ...]
};

class PythonExporter
{
public:
    static PythonExporter* get_singleton() {
        return _singleton;
    }

    PythonExporter();

    // 添加数据字段
    void addDataField(const QString& logType, const QStringList& fields);

    // 设置数据库名称
    void setDatabaseName(const QString& dbName);

    // 导出 Python 文件
    bool exportToPython(const QString& outputPath);

    // 清空所有数据字段
    void clear();

private:
    static PythonExporter* _singleton;

    QString generatePythonCode();
    QString generateVariableNames(const QString& logType, const QString &fields);

    QList<DataField> m_dataFields;
    QString m_databaseName;
};

#endif // PYTHONEXPORTER_H
