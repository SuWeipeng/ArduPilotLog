#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QThread>
#include <QFile>
#include "APLLoggingCategory.h"

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
    bool isDirExist(QString fullPath);

public slots:
    void showFile();
    void saveFile();
    void saveAsDone();

signals:
    void saveSuccess();
    void saveAsStart(const QString &dbdir);

private:
    APLRead     * _aplRead;
    QFileDialog * _qfiledialog;
    QThread     * _workThread;
    SaveAsWorker* _worker;
};

#endif // DIALOG_H
