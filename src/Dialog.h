#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include "APLLoggingCategory.h"

Q_DECLARE_LOGGING_CATEGORY(DIALOG_LOG)

class APLRead;
class QFileDialog;

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = 0);
    ~Dialog();

    APLRead* getAPLRead() const { return _aplRead; }

public slots:
    void showFile();
    void saveFile();

signals:
    void saveSuccess();

private:
    APLRead *_aplRead;
    QFileDialog *_qfiledialog;
};

#endif // DIALOG_H
