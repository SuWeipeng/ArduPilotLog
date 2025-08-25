#ifndef APLREAD_H
#define APLREAD_H

#include <QFile>
#include <QThread>
#include <QBuffer>
#include <QHash>
#include "LogStructure.h"
#include "APLLoggingCategory.h"

Q_DECLARE_LOGGING_CATEGORY(APLREAD_LOG)

typedef struct LogFormat
{
    quint8  id;
    QString name;
    QString format;
    bool    valid;
    LogFormat(){
        id     = 0;
        name   = "";
        format = "";
        valid  = true;
    }
}LFMT;

class APLDataCache;

class APLReadWorker : public QObject
{
    Q_OBJECT
public:
    explicit APLReadWorker(QObject *parent = nullptr);
    ~APLReadWorker();

private:
    APLDataCache* _dataCache;
    qint64  _fileSize;
    mutable QHash<QString, int> _formatLengthCache;
    void    _decode(const uchar* p_data, qint64 data_size);
    bool    _checkMessage(QString &name, QString &format, QString &labels) const;
    void    _decodeData(QString &format, const uchar *ptr, QString &value) const;
    int     _getMessageLength(const QString &format) const;
    bool    _checkName(QString &name) const;
    bool    _checkFormat(QString &format) const;
    bool    _checkLabels(QString &labels) const;

signals:
    void fileOpened();
    void send_process(qint64 pos, qint64 size);

public slots:
    void decodeLogFile(const QString &file_dir);
};

class APLExportWorker : public QObject
{
    Q_OBJECT
public:
    explicit APLExportWorker(QObject *parent = nullptr);
    ~APLExportWorker(){}
public slots:
    void exportCSV(const QString &file_dir);
signals:
    void saveSuccess();
};

class APLRead : public QObject
{
    Q_OBJECT

public:
    APLRead();
    ~APLRead();

    void    getDatastream(const QString &file_dir);
    QString getFileName(void) { return _file_name; }
    QString getFilePath(void) { return _file_path; }

    static APLRead* getAPLRead() { return _instance; }
    APLExportWorker*  export_worker;

signals:
    void fileOpened();
    void startRunning(const QString &file_dir);
    void startExport(const QString &file_dir);
    void saveSuccess();

public slots:
    void getFileDir(const QString &file_dir);
    void getFileOpened();
    void calc_process(qint64 pos, qint64 size);
    void exportCSV();

private:
    void            _resetDataBase();
    void            _resetFMT(int i);
    QString         _file_name;
    QString         _file_path;
    QString         _file_dir;
    QThread*        _workThread;
    APLReadWorker*  _worker;
    QThread*        _exportThread;

    static APLRead*  _instance;
};

#endif // APLREAD_H
