#ifndef APLREAD_H
#define APLREAD_H

#include "LogStructure.h"
#include "APLLoggingCategory.h"

Q_DECLARE_LOGGING_CATEGORY(APLREAD_LOG)

typedef struct LogFormat
{
    quint8  id;
    QString name;
    QString format;
    bool    valid;
    LogFormat(){
        id     = 0;
        name   = "";
        format = "";
        valid  = true;
    }
}LFMT;

class APLDB;

class APLRead : public QObject
{
    Q_OBJECT

public:
    APLRead();
    ~APLRead();

    void    getDatastream(const QString &file_dir);
    QString getFileName(void) { return _file_name; }

    static APLRead* getAPLRead() { return _instance; }

signals:
    void fileOpened();

public slots:
    void getFileDir(const QString &file_dir);

private:
    APLDB *_apldb;
    bool    _checkMessage(QString &name, QString &format, QString &labels) const;
    bool    _checkName(QString &name) const;
    bool    _checkFormat(QString &format) const;
    bool    _checkLabels(QString &labels) const;
    void    _decode(QDataStream &in) const;
    void    _decodeData(QString &format, QDataStream &in, QString &value) const;
    void    _resetDataBase();
    void    _resetFMT(int i);
    QString _file_name;

    static APLRead*  _instance;
};

#endif // APLREAD_H
