#ifndef APLREAD_H
#define APLREAD_H

#include <QFile>
#include <QThread>
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

class APLDB;

class APLReadWorker : public QObject
{
    Q_OBJECT
public:
    explicit APLReadWorker(QObject *parent = nullptr);
    ~APLReadWorker();

private:
    APLDB*  _apldb;
    qint64  _fileSize;
    void    _decode(QDataStream &in, QFile *file);
    bool    _checkMessage(QString &name, QString &format, QString &labels) const;
    void    _decodeData(QString &format, QDataStream &in, QString &value) const;
    bool    _checkName(QString &name) const;
    bool    _checkFormat(QString &format) const;
    bool    _checkLabels(QString &labels) const;

signals:
    void fileOpened();
    void send_process(qint64 pos, qint64 size);

public slots:
    void decodeLogFile(const QString &file_dir);
    void reset_db();
};

class APLRead : public QObject
{
    Q_OBJECT

public:
    APLRead();
    ~APLRead();

    void    getDatastream(const QString &file_dir);
    QString getFileName(void) { return _file_name; }

    static APLRead* getAPLRead() { return _instance; }

signals:
    void fileOpened();
    void startRunning(const QString &file_dir);
    void reset_db();

public slots:
    void getFileDir(const QString &file_dir);
    void getFileOpened();
    void calc_process(qint64 pos, qint64 size);

private:
    void            _resetDataBase();
    void            _resetFMT(int i);
    QString         _file_name;
    QThread*        _workThread;
    APLReadWorker*  _worker;

    static APLRead*  _instance;
};

#endif // APLREAD_H
