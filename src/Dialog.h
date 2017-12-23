#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QLineEdit>

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = 0);
    ~Dialog();

public slots:
    void showFile();

private:
    QLineEdit *fileLineEdit;
};

#endif // DIALOG_H
