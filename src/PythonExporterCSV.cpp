#include "PythonExporterCSV.h"
#include <QDebug>

PythonExporterCSV* PythonExporterCSV::_singleton = nullptr;

PythonExporterCSV::PythonExporterCSV()
{
    _singleton = this;
}

void PythonExporterCSV::addDataField(const QString& logType, const QStringList& fields)
{
    CSVDataField field;
    field.logType = logType;
    field.fields = fields;
    m_dataFields.append(field);
}

void PythonExporterCSV::setLogFolderName(const QString& folderName)
{
    m_logFolderName = folderName;
}

bool PythonExporterCSV::exportToPython(const QString& outputPath)
{
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法创建文件:" << outputPath;
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    QString pythonCode = generatePythonCode();
    out << pythonCode;

    file.close();
    return true;
}

void PythonExporterCSV::clear()
{
    m_dataFields.clear();
    m_logFolderName.clear();
}

QString PythonExporterCSV::generatePythonCode()
{
    QString code;
    QTextStream stream(&code);

    // 文件头部
    stream << "#!/usr/bin/python3\n";
    stream << "# -*- coding:utf-8 -*-\n";
    stream << "import numpy as np\n";
    stream << "from matplotlib import pyplot as plt\n";
    stream << "import sys\n";
    stream << "import os\n";
    stream << "from utilities.LogCSVParser import LogCSVParser\n";
    stream << "import platform\n";
    stream << "import matplotlib\n\n";

    // 平台检查
    stream << "# Check if running on Linux\n";
    stream << "if platform.system() == 'Linux':\n";
    stream << "    try:\n";
    stream << "        matplotlib.use('TkAgg')\n";
    stream << "    except ImportError:\n";
    stream << "        print(\"sudo apt-get install python3-tk\")\n";
    stream << "        # Fallback to default backend\n";
    stream << "        pass\n\n";

    // 命令行参数处理
    stream << "# Handle command line arguments\n";
    stream << "if len(sys.argv) > 1:\n";
    stream << "    log_folder_name = sys.argv[1]\n";
    stream << "else:\n";
    stream << "    log_folder_name = \"" << m_logFolderName << "\"\n\n";

    // 路径处理
    stream << "# Cross-platform path handling\n";
    stream << "log_dir_path = os.path.join('..', log_folder_name)\n\n";

    // 文件夹存在检查
    stream << "# Check if file exists\n";
    stream << "if not os.path.isdir(log_dir_path):\n";
    stream << "    print(f\"Error: Log directory not found: {log_dir_path}\")\n";
    stream << "    sys.exit(1)\n\n";

    stream << "log = LogCSVParser(log_dir_path)\n\n";

    // 数据读取部分
    stream << "try:\n";

    for (const auto& field : m_dataFields) {
        stream << generateDataReading(field);
        stream << "\n";
    }

    // 异常处理
    stream << "except (FileNotFoundError, KeyError, Exception) as e:\n";
    stream << "    print(f\"Error: Problem reading data - {e}\")\n";
    stream << "    sys.exit(1)\n";

    return code;
}

QString PythonExporterCSV::generateDataReading(const CSVDataField& field)
{
    QString code;
    QTextStream stream(&code);

    // 生成 get_data 调用
    stream << "    data = log.get_data(\"" << field.logType << "\")\n";

    // 生成变量赋值
    for (const QString& fieldName : field.fields) {
        QString varName = generateVariableNames(field.logType, fieldName);
        stream << "    " << varName << " = data['" << fieldName << "'].tolist()\n";
    }

    return code;
}

QString PythonExporterCSV::generateVariableNames(const QString& logType, const QString& field)
{
    // 将 logType 转换为小写，如 "TECS" -> "tecs", "ARSP0" -> "arsp0"
    QString prefix = logType.toLower();

    // 将 field 转换为小写，如 "TimeUS" -> "timeus", "Airspeed" -> "airspeed"
    QString suffix = field.toLower();

    return prefix + "_" + suffix;
}