#include "DialogPython.h"
#include "APLReadConf.h"
#include "mainwindow.h"
#include "APLRead.h"
#include <QFileDialog>
#include <QProcess>

APL_LOGGING_CATEGORY(DIALOGPYTHON_LOG,        "DialogPythonLog")

DialogPython::DialogPython(QWidget *parent)
    : QDialog(parent)
    , _aplReadConf(new APLReadConf)
    , _qfileDialogLoad(new QFileDialog)
{
    connect(_qfileDialogLoad,  &QFileDialog::fileSelected, _aplReadConf, &APLReadConf::getFileDir);
}

DialogPython::~DialogPython()
{
    delete _aplReadConf;
    delete _qfileDialogLoad;
}

bool DialogPython::isDirExist(QString fullPath)
{
    QDir dir(fullPath);

    if(dir.exists())
    {
        return true;
    }
    return false;
}

void DialogPython::showFile()
{
    QString path = APLRead::getAPLRead()->getFilePath();

    if (isDirExist(QString("%1/Python").arg(path)))
    {
        path = QString("%1/Python").arg(path);
    }

    QString scriptFile = _qfileDialogLoad->getOpenFileName(this
                                                           ,"open plot config"
                                                           ,path
                                                           ,"Config files(*.py)");
    QFileInfo fileInfo(scriptFile);

    QString suffix = fileInfo.suffix();
    if (suffix.compare("py", Qt::CaseSensitive) == 0) {
        QProcess *process = new QProcess(this);

        QString pythonPath = MainWindow::getMainWindow()->dialog()->get_python_path();
        QString scriptPath = scriptFile;

        QString db_name = MainWindow::getMainWindow()->dialog()->get_db_name();
        if (db_name.length() == 0) {
            QFileInfo fileInfo(MainWindow::getMainWindow()->dialog()->get_logdir());
            QString baseName = fileInfo.completeBaseName();
            QString dirPath = fileInfo.absolutePath();
            QString newExtension = "db";
            db_name = QDir(dirPath).filePath(baseName + "." + newExtension);
        }

        if (pythonPath.length() > 0) {
            // 实时输出标准输出
            connect(process, &QProcess::readyReadStandardOutput,
                    this, [process]() {
                        QByteArray data = process->readAllStandardOutput();
                        if (!data.isEmpty()) {
                            qCDebug(DIALOGPYTHON_LOG) << "Output:" << QString::fromUtf8(data);
                        }
                    });

            // 实时输出标准错误
            connect(process, &QProcess::readyReadStandardError,
                    this, [process]() {
                        QByteArray data = process->readAllStandardError();
                        if (!data.isEmpty()) {
                            qCDebug(DIALOGPYTHON_LOG) << "Error:" << QString::fromUtf8(data);
                        }
                    });

            // 进程结束时的清理
            connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                    this, [process](int exitCode, QProcess::ExitStatus exitStatus) {
                        qCDebug(DIALOGPYTHON_LOG) << "Process finished with exit code:" << exitCode;

                        // 读取可能剩余的输出
                        QByteArray remainingOutput = process->readAllStandardOutput();
                        QByteArray remainingError = process->readAllStandardError();

                        if (!remainingOutput.isEmpty()) {
                            qCDebug(DIALOGPYTHON_LOG) << "Final Output:" << QString::fromUtf8(remainingOutput);
                        }
                        if (!remainingError.isEmpty()) {
                            qCDebug(DIALOGPYTHON_LOG) << "Final Error:" << QString::fromUtf8(remainingError);
                        }

                        process->deleteLater();
                    });

            // 进程错误处理
            connect(process, &QProcess::errorOccurred,
                    this, [process](QProcess::ProcessError error) {
                        qCDebug(DIALOGPYTHON_LOG) << "Process error:" << error;
                        process->deleteLater();
                    });

            // 启动进程
            process->start(pythonPath, QStringList() << "-u" << scriptPath << db_name);

            // 可选：设置进程通道模式以确保输出不被缓冲
            process->setProcessChannelMode(QProcess::SeparateChannels);
        } else {
            process->deleteLater();
        }
    }
}

void DialogPython::saveFile()
{

}
