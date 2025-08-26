#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QThread>
#include <QFile>
#include "APLLoggingCategory.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

Q_DECLARE_LOGGING_CATEGORY(DIALOG_LOG)

class APLRead;
class QFileDialog;

class SaveAsWorker : public QObject
{
  Q_OBJECT
public:
    explicit SaveAsWorker(QObject *parent = nullptr);
    ~SaveAsWorker();

private:

signals:
    void saveAsDone();
    void send_process(qint64 pos, qint64 size);

public slots:
    void run(const QString &dbdir);
};

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = 0);
    ~Dialog();

    APLRead* getAPLRead() const { return _aplRead; }
    QString  get_db_name() const { return _db_name; }
    QString  get_python_path() const { return _python_path; }
    QString  get_logdir() const { return _logdir; }
    bool     get_python_ingnore_db() const { return _python_ingnore_db; }
    bool isDirExist(QString fullPath);
    void setTrimFrom(const quint64& v) { _trim_from = v; }
    void setTrimTo(const quint64& v) { _trim_to = v; }

public slots:
    void showFile();
    void saveFile();
    void saveAsDone();
    void trim();
    void split(bool checked);
    void ignore_db(bool checked);

signals:
    void saveSuccess();
    void saveAsStart(const QString &dbdir);
    void settingsLoaded(const QJsonObject &settings);

private:
    void loadSettings();

    APLRead     * _aplRead;
    QFileDialog * _qfiledialog;
    QThread     * _workThread;
    SaveAsWorker* _worker;
    QString       _opendir;
    QString       _filter_file;
    qint8         _filter_mode;
    QStringList   _filter_include;
    QStringList   _filter_exclude;
    bool          _table_split = false;
    quint64       _trim_from = 0;
    quint64       _trim_to = 0;
    QString       _logdir;
    QString       _db_name;
    QString       _python_path;
    bool          _python_ingnore_db;
};

#endif // DIALOG_H
