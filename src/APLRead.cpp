#include <QFile>
#include <QDataStream>
#include "APLRead.h"
#include "APLDB.h"

APL_LOGGING_CATEGORY(APLREAD_LOG,        "APLReadLog")

APLRead::APLRead()
    : _apldb(new APLDB)
{
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
            if(_apldb->checkMainTable(head_check[2])){

            }else{
                _apldb->addToMainTable();
            }
        }
    }

    qCDebug(APLREAD_LOG) << "All data have been read";

    file.close();
}
