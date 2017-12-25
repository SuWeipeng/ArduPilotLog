#ifndef APLREAD_H
#define APLREAD_H

#include "LogStructure.h"
#include "APLLoggingCategory.h"

Q_DECLARE_LOGGING_CATEGORY(APLREAD_LOG)

class APLDB;

class APLRead : public QObject
{
    Q_OBJECT

public:
    APLRead();
    ~APLRead();

    void getDatastream(const QString &file_dir);

public slots:
    void getFileDir(const QString &file_dir);

private:
    APLDB *_apldb;
    bool  _checkMessage(QString &name, QString &format, QString &labels) const;
    bool  _checkName(QString &name) const;
    bool  _checkFormat(QString &format) const;
    bool  _checkLabels(QString &labels) const;
    void  _decode(QDataStream &in) const;
    void  _decodeData(QDataStream &in, quint8 *head_check) const;
    void  _getValues(QString &format, QDataStream &in, QString &value) const;
};

#endif // APLREAD_H
