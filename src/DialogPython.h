#ifndef DIALOGPYTHON_H
#define DIALOGPYTHON_H

#include <QDialog>
#include <QLineEdit>
#include <QDir>
#include "APLLoggingCategory.h"

Q_DECLARE_LOGGING_CATEGORY(DIALOGPYTHON_LOG)

class APLReadConf;
class QFileDialog;

class DialogPython : public QDialog
{
    Q_OBJECT

public:
    DialogPython(QWidget *parent = 0);
    ~DialogPython();

    APLReadConf* getAPLReadConf() const { return _aplReadConf; }
    bool isDirExist(QString fullPath);

public slots:
    void showFile();
    void saveFile();

signals:
    void saveSuccess();

private:
    APLReadConf *_aplReadConf;
    QFileDialog *_qfileDialogLoad;
};

#endif // DIALOGPYTHON_H
