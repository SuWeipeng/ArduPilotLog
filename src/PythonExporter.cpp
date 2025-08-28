#include "PythonExporter.h"
#include <QDebug>

PythonExporter* PythonExporter::_singleton;

PythonExporter::PythonExporter()
{
    _singleton = this;
}

void PythonExporter::addDataField(const QString& logType, const QStringList& fields)
{
    DataField field;
    field.logType = logType;
    field.fields = fields;
    m_dataFields.append(field);
}

void PythonExporter::setDatabaseName(const QString& dbName)
{
    m_databaseName = dbName;
}

bool PythonExporter::exportToPython(const QString& outputPath)
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

void PythonExporter::clear()
{
    m_dataFields.clear();
    m_databaseName.clear();
}

QString PythonExporter::generatePythonCode()
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
    stream << "from utilities.LogDBParser import LogDBParser\n";
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
    stream << "    db_name = sys.argv[1]\n";
    stream << "else:\n";
    stream << "    db_name = \"" << m_databaseName << "\"\n\n";

    // 路径处理
    stream << "# Cross-platform path handling\n";
    stream << "db_path = os.path.join('..', db_name)\n\n";

    // 文件存在检查
    stream << "# Check if file exists\n";
    stream << "if not os.path.exists(db_path):\n";
    stream << "    print(f\"Error: Database file not found: {db_path}\")\n";
    stream << "    sys.exit(1)\n\n";

    stream << "log = LogDBParser(db_path)\n\n";

    // 数据读取部分
    stream << "try:\n";

    int dataIndex = 0;
    for (const auto& field : m_dataFields) {
        // 生成 getData 调用
        stream << "    data = log.getData(\"" << field.logType << "\"";
        for (const QString& fieldName : field.fields) {
            stream << ",\"" << fieldName << "\"";
        }
        stream << ")\n";

        // 生成变量赋值
        for (int i = 0; i < field.fields.size(); ++i) {
            QString varName = generateVariableNames(field.logType, field.fields[i]);
            stream << "    " << varName << " = data[" << i << "]\n";
        }
        stream << "\n";

        dataIndex++;
    }

    stream << "except Exception as e:\n";
    stream << "    print(f\"Error: Problem reading data - {e}\")\n";
    stream << "    sys.exit(1)\n";

    return code;
}

QString PythonExporter::generateVariableNames(const QString& logType, const QString& field)
{
    return logType.toLower() + "_" + field.toLower();
}
