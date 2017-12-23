#include "Dialog.h"
#include <QFileDialog>
Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    fileLineEdit=new QLineEdit;
}

Dialog::~Dialog()
{

}

void Dialog::showFile()
{
    QString s = QFileDialog::getOpenFileName(this,"open file dialog","/","Binary files(*.bin);;DB files(*.db)");
    fileLineEdit->setText(s);
}
