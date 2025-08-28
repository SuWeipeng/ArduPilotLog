#include "DialogPython.h"
#include "APLReadConf.h"
#include "APLDataCache.h"
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
    } else {
        // 创建 Python 文件夹
        QString pythonDir = QString("%1/Python").arg(path);
        QDir().mkpath(pythonDir);

        // 创建 utilities 子文件夹
        QString utilitiesDir = QString("%1/utilities").arg(pythonDir);
        QDir().mkpath(utilitiesDir);

        // 在 Python 文件夹下创建示例文件
        QFile file1(QString("%1/QuadPlane_example_01_DB.py").arg(pythonDir));
        if (file1.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file1.write(example_01.toUtf8());
            file1.close();
        }

        QFile file2(QString("%1/QuadPlane_example_02_DB.py").arg(pythonDir));
        if (file2.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file2.write(example_02_1_of_3.toUtf8());
            file2.write(example_02_2_of_3.toUtf8());
            file2.write(example_02_3_of_3.toUtf8());
            file2.close();
        }

        QFile file3(QString("%1/example_log_download.txt").arg(pythonDir));
        if (file3.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file3.write(QString("https://drive.google.com/drive/folders/1YHfcqXl9vxriioPVBlYudXmWCDvZKrsK?usp=sharing").toUtf8());
            file3.close();
        }

        // 在 utilities 文件夹下创建工具文件
        QFile parserFile(QString("%1/LogDBParser.py").arg(utilitiesDir));
        if (parserFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            parserFile.write(lib_01.toUtf8());
            parserFile.close();
        }

        QFile mathFile(QString("%1/MathCommon.py").arg(utilitiesDir));
        if (mathFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            mathFile.write(lib_02.toUtf8());
            mathFile.close();
        }

        QFile utilityFile(QString("%1/utilities.py").arg(utilitiesDir));
        if (utilityFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            utilityFile.write(lib_03.toUtf8());
            utilityFile.close();
        }

        QFile parserCSV(QString("%1/LogCSVParser.py").arg(utilitiesDir));
        if (parserCSV.open(QIODevice::WriteOnly | QIODevice::Text)) {
            parserCSV.write(lib_04.toUtf8());
            parserCSV.close();
        }

        QFile example_1csv(QString("%1/QuadPlane_example_01_CSV.py").arg(pythonDir));
        if (example_1csv.open(QIODevice::WriteOnly | QIODevice::Text)) {
            example_1csv.write(example_03.toUtf8());
            example_1csv.close();
        }
        path = pythonDir;  // 设置路径为新创建的 Python 文件夹
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
        if (db_name.length() == 0 && !MainWindow::getMainWindow()->dialog()->get_python_ingnore_db()) {
            QFileInfo fileInfo(MainWindow::getMainWindow()->dialog()->get_logdir());
            QString baseName = fileInfo.completeBaseName();
            QString dirPath = fileInfo.absolutePath();
            QString newExtension = "db";
            db_name = QDir(dirPath).filePath(baseName + "." + newExtension);
        }

        QFile dbFile(db_name);
        QDir dir;
        if (!dbFile.exists() || MainWindow::getMainWindow()->dialog()->get_python_ingnore_db()) {
            if (!MainWindow::getMainWindow()->dialog()->get_python_ingnore_db()) {
                qCDebug(DIALOGPYTHON_LOG) << QString("%1 does NOT exist! Checking CSV folder...").arg(db_name);
            }
            db_name = APLDataCache::get_singleton()->get_export_dir();
            if (db_name.length() == 0) {
                QFileInfo fileInfo(MainWindow::getMainWindow()->dialog()->get_logdir());
                QString baseName = fileInfo.completeBaseName();
                QString dirPath = fileInfo.absolutePath();
                db_name = QDir(dirPath).filePath(baseName + "_csv");
                if (MainWindow::getMainWindow()->dialog()->get_csv_mode()){
                    db_name = dirPath;
                }
            }
            if (!dir.exists(db_name)) {
                QMessageBox::information(this,tr("Information"),QString("%1 does NOT exist! Please 'Export *.db' or 'Export *.csv' first.").arg(db_name));
                return;
            }
        }


        if (pythonPath.length() > 0) {
            // 实时输出标准输出
            connect(process, &QProcess::readyReadStandardOutput,
                    this, [this, process]() {
                        QByteArray data = process->readAllStandardOutput();
                        if (!data.isEmpty()) {
                            QString message(QString::fromUtf8(data));
                            if (message.contains("Error:")){
                                QMessageBox::information(this,tr("Information"),message);
                            }
                            qCDebug(DIALOGPYTHON_LOG) << "Output:" << message;
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

            // 可选：设置进程通道模式以确保输出不被缓冲
            process->setProcessChannelMode(QProcess::SeparateChannels);

            // 启动进程
            process->start(pythonPath, QStringList() << "-u" << scriptPath << db_name);
        } else {
            process->deleteLater();
        }
    }
}
