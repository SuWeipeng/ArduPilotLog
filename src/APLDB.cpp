#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include "APLDB.h"

APL_LOGGING_CATEGORY(APLDB_LOG,        "APLDBLog")

APLDB::APLDB()
    : _Number(0)
{
    _name.clear();
    _values.clear();
    _maintable_names.clear();
    _maintable_formats.clear();
    _maintable_ids.clear();
    _maintable_item.clear();
}

APLDB::~APLDB()
{
    qCDebug(APLDB_LOG) << "APLDB::~APLDB()";
    if(isOpen()){
        close();
    }
}

void APLDB::createAPLDB(const QString &dbPath)
{
    _apldb = QSqlDatabase::addDatabase("QSQLITE");
    _apldb.setDatabaseName(dbPath);

    if(!_apldb.open()){
        qCDebug(APLDB_LOG) << _apldb.lastError();
        qCDebug(APLDB_LOG) << _apldb.drivers();
    }else{
        QSqlQuery query(_apldb);

        // SQLite 性能优化设置
	    query.exec("PRAGMA journal_mode = WAL");        // 使用WAL模式
	    query.exec("PRAGMA synchronous = NORMAL");      // 降低同步级别
        query.exec("PRAGMA cache_size = 50000");        // 增加缓存大小
	    query.exec("PRAGMA temp_store = MEMORY");       // 临时表存储在内存
	    query.exec("PRAGMA mmap_size = 268435456");     // 使用内存映射(256MB)

        query.exec("PRAGMA page_size = 65536");           // 增大页面大小
        query.exec("PRAGMA wal_autocheckpoint = 0");      // 禁用自动检查点
        query.exec("PRAGMA optimize");                    // 启用查询优化器
        query.exec("PRAGMA threads = 4");                // 启用多线程（Qt 6.2+）
        query.exec("PRAGMA locking_mode = EXCLUSIVE");    // 独占锁模式

        // 创建主表
        bool success = query.exec("CREATE TABLE maintable(LineNo INTEGER PRIMARY KEY AUTOINCREMENT, id INT8, len INT8, name VARCHAR UNIQUE, format VARCHAR, labels VARCHAR, units VARCHAR, multipliers VARCHAR)");
        if(success){
          qCDebug(APLDB_LOG) << "create maintable success";
        }else{
          qCDebug(APLDB_LOG) << "create maintable failed";
        }
    }
}

//true: id already exist
bool APLDB::checkMainTable(quint8 id)
{
    bool ret = false;
    if(_maintable_ids.contains(QString("%1").arg(id))){
        ret = true;
    }

    return ret;
}

void APLDB::addToMainTable(quint8 type,
                           quint8 len,
                           QString name,
                           QString format,
                           QString labels)
{
    if (_maintable_names.contains(name)) {
        return;
    }

    _maintable_item.append(QString("INSERT INTO maintable(id, len, name, format, labels) VALUES(%1,%2,\"%3\",\"%4\",\"%5\")")
                               .arg(type)
                               .arg(len)
                               .arg(name)
                               .arg(format)
                               .arg(labels));

    _maintable_ids.append(QString("%1").arg(type));
    _maintable_names.append(name);
    _maintable_formats.append(format);

    if(!_createSubTable(name, format, labels)){
        qCDebug(APLDB_LOG) << name << "Sub table create fail";
    }
}

void APLDB::addToSubTable(QString name, QString values)
{
    QSqlQuery query_insert;

    values = QString("%1,%2").arg(++_Number).arg(values);
    query_insert.prepare(QString("INSERT INTO %1 VALUES(%2)").arg(name, values));
    if(!query_insert.exec()){
        QSqlError queryErr = query_insert.lastError();
        qCDebug(APLDB_LOG)<<"addToSubTable"<<queryErr.text();
    }
}

void APLDB::addToSubTableBuf(QString name, QVector<QVariant> values)
{
    _name.append(name);
    _values.append(values);
}

void APLDB::buf2DB()
{
    QSqlQuery query_insert(_apldb);

    // 处理主表
    _apldb.transaction();
    for(const QString& item : _maintable_item) {
        if(!query_insert.exec(item)) {
            qCDebug(APLDB_LOG) << "MainTable insert failed:" << query_insert.lastError().text();
            _apldb.rollback();
            return;
        }
    }
    _apldb.commit();

    // 批量处理子表数据
    QMap<QString, QList<QVector<QVariant>>> grouped_data;
    for(qsizetype i = 0; i < _name.length(); ++i) {
        grouped_data[_name[i]].append(_values[i]);
    }

    for (auto it = grouped_data.constBegin(); it != grouped_data.constEnd(); ++it) {
        const QString &tableName = it.key();
        const QList<QVector<QVariant>> &rows = it.value();

        if (rows.isEmpty()) continue;

        // 使用批量插入
        insertBatchData(tableName, rows);
    }

    // 清理
    _maintable_item.clear();
    _maintable_names.clear();
    _maintable_formats.clear();
    _maintable_ids.clear();
    _name.clear();
    _values.clear();

    // 清理后进行数据库优化
    QSqlQuery query(_apldb);
    query.exec("PRAGMA optimize");
    query.exec("PRAGMA wal_checkpoint(TRUNCATE)"); // 清理WAL文件
}

void APLDB::insertBatchData(const QString& tableName, const QList<QVector<QVariant>>& rows)
{
    if (rows.isEmpty()) return;

    const int batchSize = 5000; // 每批处理5000条记录
    const int columnCount = rows.first().size();

    // 构建批量插入SQL
    QString placeholders = "?";
    for (int j = 0; j < columnCount; ++j) {
        placeholders += ", ?";
    }

    QString sql = QString("INSERT INTO \"%1\" VALUES(%2)").arg(tableName, placeholders);
    QSqlQuery query(_apldb);
    if (!query.prepare(sql)) {
        qCDebug(APLDB_LOG) << "Prepare failed:" << query.lastError().text();
        return;
    }

    qint64 lineCounter = 0;

    _apldb.transaction();

    for (int i = 0; i < rows.size(); ++i) {
        const QVector<QVariant>& row = rows[i];

        query.bindValue(0, ++lineCounter);
        for (int j = 0; j < row.size(); ++j) {
            query.bindValue(j + 1, row[j]);
        }

        if (!query.exec()) {
            qCDebug(APLDB_LOG) << "Insert failed:" << query.lastError().text();
            _apldb.rollback();
            return;
        }

        // 每5000条提交一次
        if (i % batchSize == 0 && i > 0) {
            if (!_apldb.commit()) {
                qCDebug(APLDB_LOG) << "Commit failed:" << _apldb.lastError().text();
                _apldb.rollback();
                return;
            }
            _apldb.transaction();
        }
    }

    // 提交剩余数据
    if (!_apldb.commit()) {
        qCDebug(APLDB_LOG) << "Batch commit failed:" << _apldb.lastError().text();
        _apldb.rollback();
        return;
    }
}

bool APLDB::_createSubTable(QString &name, QString &format, QString &field) const
{
    QSqlQuery query_create;
    QString   table_field = "";
    bool      success     = false;

    _createTableField(name, format, field, table_field);
    table_field = QString("%1,%2").arg("LineNo INTEGER PRIMARY KEY", table_field);
    success = query_create.exec(QString("CREATE TABLE IF NOT EXISTS \"%1\"(%2)").arg(name, table_field));

    if(!success){
        qCDebug(APLDB_LOG) << QString("CREATE TABLE IF NOT EXISTS \"%1\"(%2) FAILED: ").arg(name, table_field) << query_create.lastError().text();
    }

    return success;
}

void APLDB::_createTableField(const QString &name, QString &format, QString &field, QString &table_field) const
{
    QByteArray formatArray = format.toLatin1();
    QStringList fieldList = field.split(',');

    if(formatArray.count() != fieldList.count()){
        qCDebug(APLDB_LOG) << name << "Format and labels don't match";
        return;
    }

    for(qint8 i = 0; i < formatArray.count(); i++){
        // 使用双引号转义字段名，处理关键字冲突
        QString fieldName = QString("\"%1\"").arg(_sanitizeFieldName(name, fieldList[i]));

        QString dataType;
        switch(formatArray[i]){
        case 'a': case 'b': case 'B': case 'h': case 'H':
        case 'i': case 'I': case 'L': case 'M': case 'q': case 'Q':
            dataType = "INTEGER";
            break;
        case 'f': case 'd': case 'c': case 'C': case 'e': case 'E':
            dataType = "REAL";  // 使用REAL而不是DOUBLE
            break;
        case 'n': case 'N': case 'Z':
            dataType = "TEXT";  // 使用TEXT而不是VARCHAR
            break;
        default:
            dataType = "TEXT";
            break;
        }

        table_field += QString("%1 %2").arg(fieldName, dataType);
        if (i < formatArray.count() - 1) {
            table_field += ", ";
        }
    }
}

QString APLDB::_sanitizeFieldName(const QString &name, const QString& fieldName) const
{
    // SQLite关键字列表（部分）
    static const QStringList sqliteKeywords = {
        "ABORT", "ACTION", "ADD", "AFTER", "ALL", "ALTER", "ANALYZE", "AND", "AS", "ASC",
        "ATTACH", "AUTOINCREMENT", "BEFORE", "BEGIN", "BETWEEN", "BY", "CASCADE", "CASE",
        "CAST", "CHECK", "COLLATE", "COLUMN", "COMMIT", "CONFLICT", "CONSTRAINT", "CREATE",
        "CROSS", "CURRENT_DATE", "CURRENT_TIME", "CURRENT_TIMESTAMP", "DATABASE", "DEFAULT",
        "DEFERRABLE", "DEFERRED", "DELETE", "DESC", "DETACH", "DISTINCT", "DROP", "EACH",
        "ELSE", "END", "ESCAPE", "EXCEPT", "EXCLUSIVE", "EXISTS", "EXPLAIN", "FAIL", "FOR",
        "FOREIGN", "FROM", "FULL", "GLOB", "GROUP", "HAVING", "IF", "IGNORE", "IMMEDIATE",
        "IN", "INDEX", "INDEXED", "INITIALLY", "INNER", "INSERT", "INSTEAD", "INTERSECT",
        "INTO", "IS", "ISNULL", "JOIN", "KEY", "LEFT", "LIKE", "LIMIT", "MATCH", "NATURAL",
        "NO", "NOT", "NOTNULL", "NULL", "OF", "OFFSET", "ON", "OR", "ORDER", "OUTER", "PLAN",
        "PRAGMA", "PRIMARY", "QUERY", "RAISE", "RECURSIVE", "REFERENCES", "REGEXP", "REINDEX",
        "RELEASE", "RENAME", "REPLACE", "RESTRICT", "RIGHT", "ROLLBACK", "ROW", "SAVEPOINT",
        "SELECT", "SET", "TABLE", "TEMP", "TEMPORARY", "THEN", "TO", "TRANSACTION", "TRIGGER",
        "UNION", "UNIQUE", "UPDATE", "USING", "VACUUM", "VALUES", "VIEW", "VIRTUAL", "WHEN",
        "WHERE", "WITH", "WITHOUT"
    };

    QString sanitized = fieldName.trimmed();

    // 检查是否为关键字（不区分大小写）
    if (sqliteKeywords.contains(sanitized.toUpper())) {
        qCDebug(APLDB_LOG) << name <<  "sqliteKeywords: " << sanitized;
    }

    // 处理特殊字符
    sanitized.replace(' ', '_');
    sanitized.replace('-', '_');
    sanitized.replace('.', '_');
    sanitized.replace('/', '_');
    sanitized.replace('\\', '_');

    // 确保不以数字开头
    if (!sanitized.isEmpty() && sanitized[0].isDigit()) {
        qCDebug(APLDB_LOG) << name << "Digit start: " << sanitized;
    }

    return sanitized;
}

void APLDB::getFormat(quint8 &id, QString &name, QString &format)
{
    int idx = _maintable_ids.indexOf(QString("%1").arg(id));
    if (idx != -1) {
        name = _maintable_names[idx];
        format = _maintable_formats[idx];
    }
}

void APLDB::closeConnection()
{
    QString connection;
    connection = _apldb.connectionName();
    _apldb.close();
    _apldb = QSqlDatabase();
    QSqlDatabase::removeDatabase(connection);
}

QString APLDB::getGroupName(QSqlDatabase &db, int i)
{
    QSqlQuery query(db);

    query.exec(QString("SELECT name FROM maintable"));
    for(int n = 0; n < i; n++)
        query.next();

    return  query.value(0).toString();
}

QString APLDB::getTableName(QSqlDatabase &db, int i)
{
    QSqlQuery query(db);

    query.exec(QString("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name"));
    for(int n = 0; n < i; n++)
        query.next();

    return  query.value(0).toString();
}

int APLDB::getGroupCount(QSqlDatabase &db)
{
    QSqlQuery query(db);

    query.exec(QString("SELECT COUNT(name) FROM maintable"));
    query.next();

    return  query.value(0).toInt();
}

int APLDB::getTableNum(QSqlDatabase &db)
{
    QSqlQuery query(db);

    query.exec(QString("SELECT COUNT(*) FROM sqlite_master WHERE type='table'"));
    query.next();

    return  query.value(0).toInt();
}

int APLDB::getItemCount(QSqlDatabase &db, QString table)
{
    QSqlQuery query(db);

    query.prepare(QString("PRAGMA table_info(\"%1\")").arg(table));
    if(query.exec()){
        int count = 0;
        while(query.next()) {
            count++;
        }
        return count;
    }

    return 0;
}

QString APLDB::getItemName(QSqlDatabase &db, QString table, int i)
{
    QSqlQuery query(db);

    query.prepare(QString("PRAGMA table_info(\"%1\")").arg(table));
    if(query.exec()){
        for(int n = 0; n <= i; n++) {
            if (!query.next()) return "";
        }
        return query.value(1).toString();
    }

    return "";
}

QString APLDB::getDiff(QSqlDatabase &db, QString table, QString field)
{
    QSqlQuery query(db);
    QByteArray ret;

    query.prepare(QString("SELECT DISTINCT \"%1\" FROM \"%2\"").arg(field,table));
    if(query.exec()){
        while(query.next()){
            ret.append(query.value(0).toString().toUtf8());
            ret.append(",");
        }

        QString ret_str = QString(ret);
        if (!ret_str.isEmpty()) {
            ret_str.chop(1);
        }
        return ret_str;
    }

    return "";
}

int APLDB::getLen(QSqlDatabase &db, QString table, QString field)
{
    QSqlQuery query(db);

    query.prepare(QString("SELECT COUNT(\"%1\") FROM \"%2\"").arg(field).arg(table));

    if(!query.exec()){
        qCDebug(APLDB_LOG)<<"getLen(QSqlDatabase &db, QString table, QString field)";
        QSqlError queryErr = query.lastError();
        qCDebug(APLDB_LOG)<<queryErr.text();
        return 0;
    }

    query.next();

    return query.value(0).toInt();
}

bool APLDB::getData(QSqlDatabase &db, QString table, QString field, int len, QVector<double>& data, double offset, double scale)
{
    QSqlQuery query(db);

    query.prepare(QString("SELECT \"%1\" FROM \"%2\"").arg(field).arg(table));

    if(!query.exec()){
        QSqlError queryErr = query.lastError();
        qCDebug(APLDB_LOG)<<queryErr.text();
        return false;
    }

    for(int i=0; i<len; i++){
        query.next();
        data[i] = (query.value(0).toDouble() + offset) * scale;
    }
    return true;
}

void APLDB::getData(QSqlDatabase &db, QString table, QString field, int index, double& data)
{
    QSqlQuery query(db);

    query.prepare(QString("SELECT \"%1\" FROM \"%2\"").arg(field).arg(table));

    if(!query.exec()){
        QSqlError queryErr = query.lastError();
        qCDebug(APLDB_LOG)<<queryErr.text();
    }

    if(query.seek(index)){
        data = query.value(0).toDouble();
    }
}

void APLDB::copy_table(QSqlDatabase &db, QString new_name, QString i, int i_value, QString fields, QString origin_name)
{
    QSqlQuery query(db);

    query.prepare(QString("CREATE TABLE \"%1\" AS SELECT %2 FROM \"%3\" WHERE \"%4\"=%5").arg(new_name).arg(fields).arg(origin_name).arg(i).arg(i_value));

    if(!query.exec()){
        QSqlError queryErr = query.lastError();
    }
}

void APLDB::deleteDataBase(const QString &dbdir)
{
    if(isOpen()){
        close();
    }
    if (QFile::exists(dbdir)) {
        QFile::remove(dbdir);
    }
}

void APLDB::reset()
{
    _Number = 0;
    _name.clear();
    _values.clear();
    _maintable_names.clear();
    _maintable_formats.clear();
    _maintable_ids.clear();
    _maintable_item.clear();
}

bool APLDB::isEmpty(QSqlDatabase &db, QString table)
{
    QSqlQuery query(db);

    query.prepare(QString("SELECT COUNT(*) FROM \"%1\"").arg(table));

    if(!query.exec()){
        QSqlError queryErr = query.lastError();
        qCDebug(APLDB_LOG)<<queryErr.text();
    }
    query.next();

    if(query.value(0).toInt() == 0) return true;

    return false;
}
