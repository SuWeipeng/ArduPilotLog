#include "APLDataCache.h"
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QTextStream>

APL_LOGGING_CATEGORY(APLDATACACHE_LOG,        "APLDataCacheLog")

APLDataCache* APLDataCache::_singleton;

APLDataCache::APLDataCache(QObject *parent) : QObject(parent)
{
    _singleton = this;
    reset();
}

void APLDataCache::reset()
{
    _store.clear();
    _instantiable_store.clear();
    _binary_store.clear();
    _metadata = QJsonObject();
    _maintable_ids.clear();
    _maintable_names.clear();
    _maintable_formats.clear();
}

void APLDataCache::setTableSplit(bool enabled)
{
    _table_split = enabled;
}

void APLDataCache::setSaveCSV(bool enabled)
{
    _save_csv = enabled;
}

void APLDataCache::setTrimFrom(quint64 v)
{
    _trim_from = v;
}

void APLDataCache::setTrimTo(quint64 v)
{
    _trim_to = v;
}

void APLDataCache::setFilterMode(const qint8& v)
{
    _filter_mode = v;
}

void APLDataCache::setFilterInclude(const QStringList& v)
{
    _filter_include = v;
}

void APLDataCache::setFilterExclude(const QStringList& v)
{
    _filter_exclude = v;
}

void APLDataCache::setFilterFile(const QString& v)
{
    _filter_file = v;
}

void APLDataCache::addFormat(const quint8 &type, const QString &name, const QString &format, const QString &labels, const qint8 &i)
{
    // --- Start of Instance Splitting Logic ---
    // 仅当 _table_split 为 true 时，才执行实例拆分逻辑
    if (_table_split && i == -1) {
        if (_instantiable_store.contains(name)) {
            return; // Already registered
        }
        QStringList headers = labels.split(',');
        if (headers.size() > 1) {
            const QString &instance_header = headers[1];
            if ((instance_header.compare("I", Qt::CaseSensitive) == 0 ||
                 instance_header.compare("Instance", Qt::CaseSensitive) == 0 ||
                 instance_header.compare("C", Qt::CaseSensitive) == 0 ||
                 instance_header.compare("IMU", Qt::CaseSensitive) == 0 ||
                 instance_header.compare("Type", Qt::CaseSensitive) == 0 ||
                 instance_header.compare("Id", Qt::CaseSensitive) == 0) &&
                name.compare("EV", Qt::CaseSensitive) != 0 &&
                name.compare("MULT", Qt::CaseSensitive) != 0 &&
                name.compare("UNIT", Qt::CaseSensitive) != 0)
            {
                if (_instantiable_store.contains(name)) {
                    return; // Already registered
                }

                _maintable_ids.append(QString("%1").arg(type));
                _maintable_names.append(name);
                _maintable_formats.append(format);

                // Store for in-memory access
                MessageData newData;
                newData.type = type;
                newData.format = format;
                newData.headers = labels.split(',');
                newData.columns.resize(newData.headers.size());
                newData.labels = labels;
                _instantiable_store[name] = newData;

                // Store for JSON export
                QJsonObject formatObj;
                formatObj["format"] = format;
                formatObj["labels"] = labels;
                _metadata[name] = formatObj;

                return;
            }
        }
    }
    // --- End of Instance Splitting Logic ---

    if (_store.contains(name)) {
        return; // Already registered
    }

    _maintable_ids.append(QString("%1").arg(type));
    _maintable_names.append(name);
    _maintable_formats.append(format);

    // Store for in-memory access
    MessageData newData;
    newData.type = type;
    newData.format = format;
    newData.headers = labels.split(',');
    newData.columns.resize(newData.headers.size());
    newData.labels = labels;
    _store[name] = newData;

    // Store for JSON export
    QJsonObject formatObj;
    formatObj["format"] = format;
    formatObj["labels"] = labels;
    _metadata[name] = formatObj;
}

// This is the new function that accepts binary data and performs instance splitting.
void APLDataCache::addData(const QString &name, const QString &new_name, const uchar *payload, const qint8 &i, const int &payload_len)
{
    if (!_store.contains(name) && !_instantiable_store.contains(name)) {
        return; // Cannot add data without a format definition
    }

    if (_filter_mode == 0) {
        if (_filter_include.contains(name) == false && _store.contains(name)) {
            return;
        }
    } else if (_filter_mode == 1) {
        if (_filter_exclude.contains(name) == true) {
            return;
        }
    }

    // --- Start of Instance Splitting Logic ---
    // 仅当 _table_split 为 true 时，才执行实例拆分逻辑
    if (_table_split && _instantiable_store.contains(name)) {
        const MessageData &messageData = _instantiable_store[name];
        const QString &new_table_name = new_name;
        if (!_store.contains(new_table_name)) {
            QJsonObject js_obj = _metadata.value(name).toObject();
            addFormat(messageData.type, // Use cached type
                    new_table_name,
                    js_obj.value("format").toString(),
                    js_obj.value("labels").toString(),
                    i
                    );
        }

        if (_filter_mode == 0) {
            if (_filter_include.contains(new_name) == false) {
                return;
            }
        } else if (_filter_mode == 1) {
            if (_filter_exclude.contains(new_name) == true) {
                return;
            }
        }

        // Add data to the instance-specific table
        if (_trim_from < _trim_to) {
            if (_cut_data(static_cast<quint8>(*(payload - 1)), _trim_from, _trim_to, *reinterpret_cast<const quint64*>(payload))){
                _binary_store[new_table_name].append(QByteArray(reinterpret_cast<const char*>(payload), payload_len));
            }
            if (*reinterpret_cast<const quint64*>(payload) > _trim_to) {
                trim_complete = true;
            }
        } else {
            _binary_store[new_table_name].append(QByteArray(reinterpret_cast<const char*>(payload), payload_len));
        }
        return;
    }
    // --- End of Instance Splitting Logic ---

    if (_trim_from < _trim_to) {
        if (_cut_data(static_cast<quint8>(*(payload - 1)), _trim_from, _trim_to, *reinterpret_cast<const quint64*>(payload))){
            _binary_store[name].append(QByteArray(reinterpret_cast<const char*>(payload), payload_len));
        }
    } else {
        _binary_store[name].append(QByteArray(reinterpret_cast<const char*>(payload), payload_len));
    }
}

QVector<double> APLDataCache::getColumn(const QString &messageName, const QString &columnName)
{
    if (!_store.contains(messageName)) {
        return QVector<double>();
    }

    const MessageData &messageData = _store[messageName];
    int columnIndex = messageData.headers.indexOf(columnName);

    if (columnIndex == -1) {
        return QVector<double>();
    }

    return messageData.columns[columnIndex];
}

void APLDataCache::exportToFile(const QString &outputDir)
{
    if (!_save_csv && !export_csv) return;
    export_csv = false;

    QString export_dir(outputDir);

    if(_filter_file.length()>0) {
        export_dir.append("_");
        export_dir.append(_filter_file.section('.',0,0));
        export_dir.append("-");
        export_dir.append(QString("%1").arg(_filter_mode));
    }

    if (_trim_from < _trim_to) {
        export_dir.append("_trim");
    }
    
    QDir dir;
    if (!dir.exists(export_dir)) {
        dir.mkpath(export_dir);
    }

    // Export data to CSV files
    for (auto it = _binary_store.constBegin(); it != _binary_store.constEnd(); ++it) {
        const QString &tableName = it.key();
        const QList<QByteArray> &rows = it.value();

        if (!_store.contains(tableName)) continue;

        QFile file(QDir(export_dir).filePath(tableName + ".csv"));
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) continue;

        QTextStream stream(&file);
        stream << _store[tableName].headers.join(',') << "\n";

        int idx = _maintable_names.indexOf(tableName);
        if (idx == -1) continue;
        QString format = _maintable_formats[idx];

        for (const QByteArray &row : rows) {
            QStringList rowValues;
            const uchar *ptr = reinterpret_cast<const uchar *>(row.constData());

            for (QChar formatChar : format) {
                switch(formatChar.toLatin1()) {
                    case 'b': { qint8 v; memcpy(&v, ptr, sizeof(v)); ptr += sizeof(v); rowValues << QString::number(v); break; }
                    case 'B': { quint8 v; memcpy(&v, ptr, sizeof(v)); ptr += sizeof(v); rowValues << QString::number(v); break; }
                    case 'h': { qint16 v; memcpy(&v, ptr, sizeof(v)); ptr += sizeof(v); rowValues << QString::number(v); break; }
                    case 'H': { quint16 v; memcpy(&v, ptr, sizeof(v)); ptr += sizeof(v); rowValues << QString::number(v); break; }
                    case 'i': { qint32 v; memcpy(&v, ptr, sizeof(v)); ptr += sizeof(v); rowValues << QString::number(v); break; }
                    case 'I': { quint32 v; memcpy(&v, ptr, sizeof(v)); ptr += sizeof(v); rowValues << QString::number(v); break; }
                    case 'f': { float v; memcpy(&v, ptr, sizeof(v)); ptr += sizeof(v); rowValues << QString::number(v); break; }
                    case 'd': { double v; memcpy(&v, ptr, sizeof(v)); ptr += sizeof(v); rowValues << QString::number(v); break; }
                    case 'c': { qint16 v; memcpy(&v, ptr, sizeof(v)); ptr += sizeof(v); rowValues << QString::number(v / 100.0); break; }
                    case 'C': { quint16 v; memcpy(&v, ptr, sizeof(v)); ptr += sizeof(v); rowValues << QString::number(v / 100.0); break; }
                    case 'e': { qint32 v; memcpy(&v, ptr, sizeof(v)); ptr += sizeof(v); rowValues << QString::number(v / 100.0); break; }
                    case 'E': { quint32 v; memcpy(&v, ptr, sizeof(v)); ptr += sizeof(v); rowValues << QString::number(v / 100.0); break; }
                    case 'L': { qint32 v; memcpy(&v, ptr, sizeof(v)); ptr += sizeof(v); rowValues << QString::number(v); break; }
                    case 'M': { quint8 v; memcpy(&v, ptr, sizeof(v)); ptr += sizeof(v); rowValues << QString::number(v); break; }
                    case 'q': { qint64 v; memcpy(&v, ptr, sizeof(v)); ptr += sizeof(v); rowValues << QString::number(v); break; }
                    case 'Q': { quint64 v; memcpy(&v, ptr, sizeof(v)); ptr += sizeof(v); rowValues << QString::number(v); break; }
                    case 'n': { char v[5] = {0}; memcpy(v, ptr, 4); ptr += 4; rowValues << v; break; }
                    case 'N': { char v[17] = {0}; memcpy(v, ptr, 16); ptr += 16; rowValues << v; break; }
                    case 'Z': { char v[65] = {0}; memcpy(v, ptr, 64); ptr += 64; rowValues << v; break; }
                    case 'a': { ptr += 64; rowValues << "(array)"; break; } // Placeholder for array
                }
            }
            stream << rowValues.join(',') << "\n";
        }
        file.close();
    }

    // Export metadata to json file
    QFile jsonFile(QDir(export_dir).filePath("metadata.json"));
    if (jsonFile.open(QIODevice::WriteOnly)) {
        jsonFile.write(QJsonDocument(_metadata).toJson());
        jsonFile.close();
    }
}

int APLDataCache::getTableNum()
{
    return _store.size();
}

int APLDataCache::getGroupCount()
{
    return _store.size();
}

QString APLDataCache::getGroupName(int i)
{
    return _store.keys()[i];
}

QString APLDataCache::getTableName(int i)
{
    return _store.keys()[i];
}

QString APLDataCache::getItemName(QString table, int i)
{
    return _store[table].headers[i];
}

int APLDataCache::getItemCount(QString table)
{
    return _store[table].headers.size();
}

bool APLDataCache::getData(QString table, QString field, int len, QVector<double>& data, double offset, double scale)
{
    if (!_binary_store.contains(table) || !_store.contains(table)) {
        return false;
    }

    const MessageData &messageData = _store[table];
    const QList<QByteArray> &binary_rows = _binary_store[table];

    int field_idx = messageData.headers.indexOf(field);
    if (field_idx == -1) {
        return false;
    }

    int idx = _maintable_names.indexOf(table);
    if (idx == -1) {
        return false;
    }
    QString format = _maintable_formats[idx];

    // Calculate the byte offset of the desired field within a binary row
    int field_offset = 0;
    bool found_field = false;
    for (int i = 0; i < format.length(); ++i) {
        if (i == field_idx) {
            found_field = true;
            break;
        }
        switch(format[i].toLatin1()) {
            case 'a': field_offset += 64; break;
            case 'b': case 'B': case 'M': field_offset += 1; break;
            case 'h': case 'H': case 'c': case 'C': field_offset += 2; break;
            case 'i': case 'I': case 'f': case 'e': case 'E': case 'L': field_offset += 4; break;
            case 'd': case 'q': case 'Q': field_offset += 8; break;
            case 'n': field_offset += 4; break;
            case 'N': field_offset += 16; break;
            case 'Z': field_offset += 64; break;
            default: return false; // Unknown format char
        }
    }

    if (!found_field) {
        return false;
    }

    data.resize(len);

    // Now, iterate through the binary rows and parse just the required field
    for (int i = 0; i < len; ++i) {
        const uchar* row_ptr = reinterpret_cast<const uchar*>(binary_rows[i].constData()) + field_offset;
        double val = 0.0;

        switch(format[field_idx].toLatin1()) {
            case 'b': { qint8 v; memcpy(&v, row_ptr, sizeof(v)); val = v; break; }
            case 'B': { quint8 v; memcpy(&v, row_ptr, sizeof(v)); val = v; break; }
            case 'h': { qint16 v; memcpy(&v, row_ptr, sizeof(v)); val = v; break; }
            case 'H': { quint16 v; memcpy(&v, row_ptr, sizeof(v)); val = v; break; }
            case 'i': { qint32 v; memcpy(&v, row_ptr, sizeof(v)); val = v; break; }
            case 'I': { quint32 v; memcpy(&v, row_ptr, sizeof(v)); val = v; break; }
            case 'f': { float v; memcpy(&v, row_ptr, sizeof(v)); val = v; break; }
            case 'd': { double v; memcpy(&v, row_ptr, sizeof(v)); val = v; break; }
            case 'c': { qint16 v; memcpy(&v, row_ptr, sizeof(v)); val = v / 100.0; break; }
            case 'C': { quint16 v; memcpy(&v, row_ptr, sizeof(v)); val = v / 100.0; break; }
            case 'e': { qint32 v; memcpy(&v, row_ptr, sizeof(v)); val = v / 100.0; break; }
            case 'E': { quint32 v; memcpy(&v, row_ptr, sizeof(v)); val = v / 100.0; break; }
            case 'L': { qint32 v; memcpy(&v, row_ptr, sizeof(v)); val = v; break; }
            case 'M': { quint8 v; memcpy(&v, row_ptr, sizeof(v)); val = v; break; }
            case 'q': { qint64 v; memcpy(&v, row_ptr, sizeof(v)); val = v; break; }
            case 'Q': { quint64 v; memcpy(&v, row_ptr, sizeof(v)); val = v; break; }
            // 'a', 'n', 'N', 'Z' are not convertible to a single double
            default: val = 0.0; break;
        }
        data[i] = (val + offset) * scale;
    }

    return true;
}

int APLDataCache::getLen(QString table, QString field)
{
    Q_UNUSED(field);
    return _binary_store.value(table).size();
}

void APLDataCache::getFormat(quint8 &id, QString &name, QString &format)
{
    int idx = _maintable_ids.indexOf(QString("%1").arg(id));
    name = _maintable_names[idx];
    format = _maintable_formats[idx];
}

bool APLDataCache::checkMainTable(quint8 id)
{
    bool ret = false;
    if(_maintable_ids.contains(QString("%1").arg(id))){
        ret = true;
    }

    return ret;
}

QVector<QVariant> APLDataCache::parseBinaryData(const QByteArray& data, const QString& format) const
{
    QVector<QVariant> result;
    const uchar *ptr = reinterpret_cast<const uchar*>(data.constData());
    int current_offset = 0;

    for (QChar formatChar : format) {
        switch(formatChar.toLatin1()) {
            case 'b': { qint8 v; memcpy(&v, ptr + current_offset, sizeof(v)); result.append(v); current_offset += sizeof(v); break; }
            case 'B': { quint8 v; memcpy(&v, ptr + current_offset, sizeof(v)); result.append(v); current_offset += sizeof(v); break; }
            case 'h': { qint16 v; memcpy(&v, ptr + current_offset, sizeof(v)); result.append(v); current_offset += sizeof(v); break; }
            case 'H': { quint16 v; memcpy(&v, ptr + current_offset, sizeof(v)); result.append(v); current_offset += sizeof(v); break; }
            case 'i': { qint32 v; memcpy(&v, ptr + current_offset, sizeof(v)); result.append(v); current_offset += sizeof(v); break; }
            case 'I': { quint32 v; memcpy(&v, ptr + current_offset, sizeof(v)); result.append(v); current_offset += sizeof(v); break; }
            case 'f': { float v; memcpy(&v, ptr + current_offset, sizeof(v)); result.append(v); current_offset += sizeof(v); break; }
            case 'd': { double v; memcpy(&v, ptr + current_offset, sizeof(v)); result.append(v); current_offset += sizeof(v); break; }
            case 'c': { qint16 v; memcpy(&v, ptr + current_offset, sizeof(v)); result.append(v / 100.0); current_offset += sizeof(v); break; }
            case 'C': { quint16 v; memcpy(&v, ptr + current_offset, sizeof(v)); result.append(v / 100.0); current_offset += sizeof(v); break; }
            case 'e': { qint32 v; memcpy(&v, ptr + current_offset, sizeof(v)); result.append(v / 100.0); current_offset += sizeof(v); break; }
            case 'E': { quint32 v; memcpy(&v, ptr + current_offset, sizeof(v)); result.append(v / 100.0); current_offset += sizeof(v); break; }
            case 'L': { qint32 v; memcpy(&v, ptr + current_offset, sizeof(v)); result.append(v); current_offset += sizeof(v); break; }
            case 'M': { quint8 v; memcpy(&v, ptr + current_offset, sizeof(v)); result.append(v); current_offset += sizeof(v); break; }
            case 'q': { qint64 v; memcpy(&v, ptr + current_offset, sizeof(v)); result.append(v); current_offset += sizeof(v); break; }
            case 'Q': { quint64 v; memcpy(&v, ptr + current_offset, sizeof(v)); result.append(v); current_offset += sizeof(v); break; }
            case 'n': { char str[5] = {0}; memcpy(str, ptr + current_offset, 4); result.append(QString(str)); current_offset += 4; break; }
            case 'N': { char str[17] = {0}; memcpy(str, ptr + current_offset, 16); result.append(QString(str)); current_offset += 16; break; }
            case 'Z': { char str[65] = {0}; memcpy(str, ptr + current_offset, 64); result.append(QString(str)); current_offset += 64; break; }
            case 'a': { current_offset += 64; result.append(QVariant()); break; } // Placeholder for array, append empty QVariant
            default: result.append(QVariant()); break; // Unknown format char, append empty QVariant
        }
    }
    return result;
}

bool
APLDataCache::_cut_data(quint8 id, quint64 start_time, quint64 stop_time, quint64 now)
{
    bool res = true;
    switch(id){
    case 32: // PARM
    case 108: // FMTU
        break;
    default:
        if(now < start_time || now > stop_time){
            res = false;
        }
    }

    return res;
}
