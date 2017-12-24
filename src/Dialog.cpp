#include "Dialog.h"
#include "APLRead.h"
#include <QFileDialog>

APL_LOGGING_CATEGORY(DIALOG_LOG,        "DialogLog")

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , _aplRead(new APLRead)
    , _qfiledialog(new QFileDialog)
{
    connect(_qfiledialog,  &QFileDialog::fileSelected, _aplRead, &APLRead::getFileDir);
}

Dialog::~Dialog()
{
    delete _aplRead;
    delete _qfiledialog;
}

void Dialog::showFile()
{
    QString logdir = _qfiledialog->getOpenFileName(this
                                                  ,"open ArduPilot binary log file"
                                                  ,"/"
                                                  ,"Binary files(*.bin);;DB files(*.db)");
    emit _qfiledialog->fileSelected(logdir);

    qCDebug(DIALOG_LOG) << logdir;
}
