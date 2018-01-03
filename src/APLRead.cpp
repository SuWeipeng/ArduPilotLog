#include <QFile>
#include <QDataStream>
#include <QRegExpValidator>
#include "APLRead.h"
#include "APLDB.h"

APL_LOGGING_CATEGORY(APLREAD_LOG,        "APLReadLog")

#define NAME_LEN    4
#define FORMAT_LEN 16
#define LABELS_LEN 64

static LFMT FMT[256]; // 256 at least

APLRead::APLRead()
    : _apldb(new APLDB)
{
    _resetDataBase();
}

APLRead::~APLRead()
{
    delete _apldb;
}

void APLRead::_resetDataBase()
{
    for(int i=0; i<256; i++)
        _resetFMT(i);
    _apldb->deleteDataBase();
    _apldb->createAPLDB();
}

void  APLRead::_resetFMT(int i)
{
    FMT[i].id     = 0;
    FMT[i].name   = "";
    FMT[i].format = "";
    FMT[i].valid  = true;
}

void APLRead::getFileDir(const QString &file_dir)
{
    _resetDataBase();
    getDatastream(file_dir);
}

void APLRead::getDatastream(const QString &file_dir)
{
    QFile  file;

    file.setFileName(file_dir);
    if(!file.open(QIODevice::ReadOnly))
    {
         qCDebug(APLREAD_LOG)<< "can not open file!";
         return;
    }

    _decode(QDataStream(&file));
    _apldb->commit();
    emit fileOpened();
    qCDebug(APLREAD_LOG) << "All data have been read";

    file.close();
}

void APLRead::_decode(QDataStream &in) const
{
    quint8  head_check[3];
    quint8  currentByte;
    quint8  ptr_pos = 0;

    while(!in.atEnd())
    {
        in >> currentByte;

        if(ptr_pos<3){
            ptr_pos++;
        }

        if (ptr_pos >= 3){
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

                if( _checkMessage(log_name,log_format,log_labels)){
                    _apldb->addToMainTable(type,
                                           len,
                                           log_name,
                                           log_format,
                                           log_labels);
                }
            }else{
                quint8 id = head_check[2];
                if(FMT[id].name.isEmpty() && FMT[id].format.isEmpty() && FMT[id].valid){
                    if(_apldb->checkMainTable(id))
                    {
                        _apldb->getFormat(id, FMT[id].name, FMT[id].format);
                    }else{
                        FMT[id].valid = false;
                    }
                }

                if(FMT[id].valid && !FMT[id].format.isEmpty()){
                    QString value_str  = "";
                    _decodeData(FMT[id].format, in, value_str);
                    _apldb->addToSubTable(FMT[id].name, value_str);
                }
            }
        }
    }
}

void APLRead::_decodeData(QString &format, QDataStream &in, QString &value) const
{
    QByteArray formatArray = format.toLatin1();

    for(qint8 i = 0; i< formatArray.count(); i++){
        switch(formatArray[i]){
        case 'a': //int16_t[32]
            qint16 v16_32[32];
            memset(v16_32, 0, sizeof(v16_32));
            in.readRawData((char *)v16_32, sizeof(v16_32));
            for(int i=0; i<32; i++)
                if(i == 31)
                    value = QString ("%1%2,").arg(value).arg(QString(v16_32[i]));
                else
                    value = QString ("%1%2 ").arg(value).arg(QString(v16_32[i]));
            break;
        case 'b': //int8_t
            qint8 v8;
            in.readRawData((char *)&v8, 1);
            value = QString ("%1%2,").arg(value).arg(v8);
            break;
        case 'B': //uint8_t
            quint8 vu8;
            in.readRawData((char *)&vu8, 1);
            value = QString ("%1%2,").arg(value).arg(vu8);
            break;
        case 'h': //int16_t
            qint16 v16;
            in.readRawData((char *)&v16, 2);
            value = QString ("%1%2,").arg(value).arg(v16);
            break;
        case 'H': //uint16_t
            quint16 vu16;
            in.readRawData((char *)&vu16, 2);
            value = QString ("%1%2,").arg(value).arg(vu16);
            break;
        case 'i': //int32_t
            qint32 v32;
            in.readRawData((char *)&v32, 4);
            value = QString ("%1%2,").arg(value).arg(v32);
            break;
        case 'I': //uint32_t
            quint32 vu32;
            in.readRawData((char *)&vu32, 4);
            value = QString ("%1%2,").arg(value).arg(v32);
            break;
        case 'f': //float
            float vf;
            in.readRawData((char *)&vf, 4);
            value = QString ("%1%2,").arg(value).arg(vf);
            break;
        case 'd': //double
            double vd;
            in.readRawData((char *)&vd, 8);
            value = QString ("%1%2,").arg(value).arg(vf);
            break;
        case 'n': //char[4]
            char vc_4[4+1];
            memset(vc_4, 0, sizeof(vc_4));
            in.readRawData(vc_4, sizeof(char)*4);
            value = QString ("%1%2,").arg(value).arg(QString(QByteArray(vc_4)));
            break;
        case 'N': //char[16]
            char vc_16[16+1];
            memset(vc_16, 0, sizeof(vc_16));
            in.readRawData(vc_16, sizeof(char)*16);
            value = QString ("%1%2,").arg(value).arg(QString(QByteArray(vc_16)));
            break;
        case 'Z': //char[64]
            char vc_64[64+1];
            memset(vc_64, 0, sizeof(vc_64));
            in.readRawData(vc_64, sizeof(char)*64);
            value = QString ("%1%2,").arg(value).arg(QString(QByteArray(vc_64)));
            break;
        case 'c': //int16_t * 100
            qint16 v16x100;
            in.readRawData((char *)&v16x100, 2);
            value = QString ("%1%2,").arg(value).arg(v16x100);
            break;
        case 'C': //uint16_t * 100
            quint16 vu16x100;
            in.readRawData((char *)&vu16x100, 2);
            value = QString ("%1%2,").arg(value).arg(vu16x100);
            break;
        case 'e': //int32_t * 100
            qint32 v32x100;
            in.readRawData((char *)&v32x100, 4);
            value = QString ("%1%2,").arg(value).arg(v32x100);
            break;
        case 'E': //uint32_t * 100
            quint32 vu32x100;
            in.readRawData((char *)&vu32x100, 4);
            value = QString ("%1%2,").arg(value).arg(vu32x100);
            break;
        case 'L': //int32_t latitude/longitude
            qint32 v32l;
            in.readRawData((char *)&v32l, 4);
            value = QString ("%1%2,").arg(value).arg(v32l);
            break;
        case 'M': //uint8_t flight mode
            quint8 vu8m;
            in.readRawData((char *)&vu8m, 1);
            value = QString ("%1%2,").arg(value).arg(vu8m);
            break;
        case 'q': //int64_t
            qint64 v64;
            in.readRawData((char *)&v64, 8);
            value = QString ("%1%2,").arg(value).arg(v64);
            break;
        case 'Q': //uint64_t
            quint64 vu64;
            in.readRawData((char *)&vu64, 8);
            value = QString ("%1%2,").arg(value).arg(vu64);
            break;
        }
    }
    value.chop(1);

}
bool APLRead::_checkMessage(QString &name, QString &format, QString &labels) const
{
    return _checkName(name) && _checkFormat(format) && _checkLabels(labels);
}

bool APLRead::_checkName(QString &name) const
{
    QRegExp reg("^[A-Z0-9]{1,4}$");
    QRegExpValidator validator(reg,0);

    int pos = 0;
    if(QValidator::Acceptable!=validator.validate(name,pos)){
        return false;
    }

    return true;
}

bool APLRead::_checkFormat(QString &format) const
{
    QRegExp reg("^[A-Za-z]{1,16}$");
    QRegExpValidator validator(reg,0);

    int pos = 0;
    if(QValidator::Acceptable!=validator.validate(format,pos)){
        return false;
    }

    return true;
}

bool APLRead::_checkLabels(QString &labels) const
{
    QRegExp reg("^[A-Za-z0-9,]{1,64}$");
    QRegExpValidator validator(reg,0);

    int pos = 0;
    if(QValidator::Acceptable!=validator.validate(labels,pos)){
        return false;
    }

    return true;
}
