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
    if(_apldb->isOpen()){
        _apldb->close();
    }
    QFile::remove(DB_FILE);
    _apldb->createAPLDB();
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

    qCDebug(APLREAD_LOG) << "All data have been read";

    file.close();
}

void APLRead::_decode(QDataStream &in) const
{
    quint8 head_check[3];
    quint8 currentByte;
    quint8 ptr_pos = 0;

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

                if( _checkMessage(log_name,log_format,log_labels)){
                    _apldb->addToMainTable(type,
                                           len,
                                           log_name,
                                           log_format,
                                           log_labels);
                }
            }else{
                _decodeData(in, head_check);
            }
        }
    }
}

void APLRead::_decodeData(QDataStream &in, quint8 *head_check) const
{
    QString value_str  = "";
    quint8  id = head_check[2];

    // 下面这个if很影响速度
    if(FMT[id].format.isEmpty() && FMT[id].valid){
        if(_apldb->checkMainTable(id))
        {
            FMT[id].format = _apldb->getFormat(id);
        }else{
            FMT[id].valid = false;
        }
    }

    if(FMT[id].valid){
        _getValues(FMT[id].format, in, value_str);
    }
}

void APLRead::_getValues(QString &format, QDataStream &in, QString &value) const
{
    QByteArray formatArray = format.toLatin1();

    for(qint8 i = 0; i< formatArray.count(); i++){
        switch(formatArray[i]){
        case 'a': //int16_t[32]

            break;
        case 'b': //int8_t

            break;
        case 'B': //uint8_t

            break;
        case 'h': //int16_t

            break;
        case 'H': //uint16_t

            break;
        case 'i': //int32_t

            break;
        case 'I': //uint32_t

            break;
        case 'f': //float

            break;
        case 'd': //double

            break;
        case 'n': //char[4]

            break;
        case 'N': //char[16]

            break;
        case 'Z': //char[64]

            break;
        case 'c': //int16_t * 100

            break;
        case 'C': //uint16_t * 100

            break;
        case 'e': //int32_t * 100

            break;
        case 'E': //uint32_t * 100

            break;
        case 'L': //int32_t latitude/longitude

            break;
        case 'M': //uint8_t flight mode

            break;
        case 'q': //int64_t

            break;
        case 'Q': //uint64_t

            break;
        }
    }
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
