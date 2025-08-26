#ifndef DialogLoad_H
#define DialogLoad_H

#include <QDialog>
#include <QLineEdit>
#include <QDir>
#include "APLLoggingCategory.h"

Q_DECLARE_LOGGING_CATEGORY(DialogLoad_LOG)

class APLReadConf;
class QFileDialog;

class DialogLoad : public QDialog
{
    Q_OBJECT

public:
    DialogLoad(QWidget *parent = 0);
    ~DialogLoad();

    APLReadConf* getAPLReadConf() const { return _aplReadConf; }
    bool isDirExist(QString fullPath);

public slots:
    void showFile();

signals:
    void saveSuccess();

private:
    APLReadConf *_aplReadConf;
    QFileDialog *_qfileDialogLoad;
};

#endif // DialogLoad_H
