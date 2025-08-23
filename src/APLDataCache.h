#ifndef APLDATACACHE_H
#define APLDATACACHE_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QJsonObject>
#include "APLLoggingCategory.h"

Q_DECLARE_LOGGING_CATEGORY(APLDATACACHE_LOG)

// 存储一种消息（如 "ATT"）的所有列数据
struct MessageData
{
    quint8 type;
    QString format;
    QStringList headers;            // 列名: "TimeUS", "Roll", "Pitch"
    QVector<QVector<double>> columns; // 列数据
    QString labels;
};

class APLDataCache : public QObject
{
    Q_OBJECT
public:
    static APLDataCache* get_singleton() {
        return _singleton;
    }

    explicit APLDataCache(QObject *parent = nullptr);

    // 添加一个新的消息格式定义
    void addFormat(const quint8 &type, const QString &name, const QString &format, const QString &labels, const qint8 &i=-1);

    // 添加一行解码后的数据（字符串形式）
    void addData(const QString &name, const QString &new_name, const uchar *payload, const qint8 &i, const int &payload_len);

    // 获取绘图所需的数据列
    QVector<double> getColumn(const QString &messageName, const QString &columnName);

    // 将缓存的数据写入文件
    void exportToFile(const QString &outputDir);

    int getTableNum();
    int getGroupCount();
    QString getGroupName(int i);
    QString getTableName(int i);
    QString getItemName(QString table, int i);
    int getItemCount(QString table);
    bool getData(QString table, QString field, int len, QVector<double>& data, double offset = 0, double scale = 1);
    int getLen(QString table, QString field);
    void getFormat(quint8 &id, QString &name, QString &format);
    bool checkMainTable(quint8 id); //true: id already exist
    const QMap<QString, MessageData>& getStore() const { return _store; }
    const QMap<QString, QList<QByteArray>>& getBinaryStore() const { return _binary_store; }
    QVector<QVariant> parseBinaryData(const QByteArray& data, const QString& format) const;
    void reset();

    void setTableSplit(bool enabled);
    void setSaveCSV(bool enabled);
    void setTrimFrom(quint64 v);
    void setTrimTo(quint64 v);
    void setFilterMode(const qint8& v);
    void setFilterInclude(const QStringList& v);
    void setFilterExclude(const QStringList& v);

private:
    static APLDataCache* _singleton;

    QMap<QString, MessageData> _store;     // 内存存储核心
    QMap<QString, MessageData> _instantiable_store;
    QMap<QString, QList<QByteArray>> _binary_store; // 新增的二进制数据仓库
    QJsonObject                _metadata;  // 用于生成 metadata.json
    QStringList                _maintable_ids;
    QStringList                _maintable_names;
    QStringList                _maintable_formats;
    bool                       _table_split = false;
    bool                       _save_csv = false;
    quint64                    _trim_from = 0;
    quint64                    _trim_to = 0;
    qint8                      _filter_mode = -1;
    QStringList                _filter_include;
    QStringList                _filter_exclude;

    bool    _cut_data(quint8 id, quint64 start_time, quint64 stop_time, quint64 now);
};

#endif // APLDATACACHE_H
