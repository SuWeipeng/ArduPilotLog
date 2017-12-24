#ifndef APLREAD_H
#define APLREAD_H

#include "LogStructure.h"
#include "APLLoggingCategory.h"

Q_DECLARE_LOGGING_CATEGORY(APLREAD_LOG)

class APLRead : public QObject
{
    Q_OBJECT

public:
    APLRead();

    // get instance
    static APLRead *instance(void) {
        return _instance;
    }

    void getDatastream(const QString &file_dir);

public slots:
    void getFileDir(const QString &file_dir);

private:
    static APLRead *_instance;
};

#endif // APLREAD_H
