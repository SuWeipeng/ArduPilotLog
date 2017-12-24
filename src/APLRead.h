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
};

#endif // APLREAD_H
