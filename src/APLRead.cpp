#include <QFile>
#include <QDataStream>
#include <QRegExpValidator>
#include "APLRead.h"
#include "APLDB.h"

APL_LOGGING_CATEGORY(APLREAD_LOG,        "APLReadLog")

#define NAME_LEN    4
#define FORMAT_LEN 16
#define LABELS_LEN 64

APLRead::APLRead()
    : _apldb(new APLDB)
{
    QFile::remove(DB_FILE);
    _apldb->createAPLDB();
}

APLRead::~APLRead()
{
    delete _apldb;
}

void APLRead::getFileDir(const QString &file_dir)
{
    getDatastream(file_dir);
}

void APLRead::getDatastream(const QString &file_dir)
{
    QFile  file;
    quint8 head_check[3];
    quint8 currentByte;
    quint8 ptr_pos = 0;

    file.setFileName(file_dir);
    if(!file.open(QIODevice::ReadOnly))
    {
         qCDebug(APLREAD_LOG)<< "can not open file!";
         return;
    }

    QDataStream in(&file);

    while(!in.atEnd())
    {
        in >> currentByte;

        if(ptr_pos<3){
            ptr_pos++;
        }

        if (ptr_pos > 3){
            head_check[0] = head_check[1];
            head_check[1] = head_check[2];
            head_check[2] = currentByte;
        }else{
            switch(ptr_pos){
            case 1:
                head_check[0] = currentByte;
                continue;
            case 2:
                head_check[1] = currentByte;
                continue;
            case 3:
                head_check[2] = currentByte;
            }
        }
        if (head_check[0]==HEAD_BYTE1 && head_check[1]==HEAD_BYTE2)
        {
            if(head_check[2] == LOG_FORMAT_MSG){
                quint8 type;
                in >> type;
                quint8 len;
                char name[NAME_LEN+1];
                char format[FORMAT_LEN+1];
                char labels[LABELS_LEN+1];
                QString log_name;
                QString log_format;
                QString log_labels;

                memset(name,0,sizeof(name));
                memset(format,0,sizeof(format));
                memset(labels,0,sizeof(labels));

                in >> len;
                in.readRawData(name  , NAME_LEN );
                in.readRawData(format, FORMAT_LEN);
                in.readRawData(labels, LABELS_LEN);
                log_name   = QString(name);
                log_format = QString(format);
                log_labels = QString(labels);

                if( _checkName(log_name)
                 && _checkFormat(log_format)
                 && _checkLabels(log_labels)){
                    _apldb->addToMainTable(type,
                                           len,
                                           log_name,
                                           log_format,
                                           log_labels);
                    qCDebug(APLREAD_LOG)<< log_name << log_format << log_labels;
                }
            }else{

            }
        }
    }

    qCDebug(APLREAD_LOG) << "All data have been read";

    file.close();
}

bool APLRead::_checkName(QString &name)
{
    QRegExp reg("^[A-Z0-9]{1,4}$");
    QRegExpValidator validator(reg,0);

    int pos = 0;
    if(QValidator::Acceptable!=validator.validate(name,pos)){
        return false;
    }

    return true;
}

bool APLRead::_checkFormat(QString &format)
{
    QRegExp reg("^[A-Za-z]{1,16}$");
    QRegExpValidator validator(reg,0);

    int pos = 0;
    if(QValidator::Acceptable!=validator.validate(format,pos)){
        return false;
    }

    return true;
}

bool APLRead::_checkLabels(QString &labels)
{
    QRegExp reg("^[A-Za-z0-9,]{1,64}$");
    QRegExpValidator validator(reg,0);

    int pos = 0;
    if(QValidator::Acceptable!=validator.validate(labels,pos)){
        return false;
    }

    return true;
}
