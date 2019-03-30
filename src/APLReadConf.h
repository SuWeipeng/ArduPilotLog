#ifndef APLREADCONF_H
#define APLREADCONF_H

#include "APLLoggingCategory.h"

Q_DECLARE_LOGGING_CATEGORY(APLREAD_CONF_LOG)

class APLReadConf : public QObject
{
    Q_OBJECT

public:
    APLReadConf();
    ~APLReadConf();

    void    getDatastream(const QString &file_dir);
    QString getFileName(void) { return _file_name; }

signals:
    void fileOpened();

public slots:
    void getFileDir(const QString &file_dir);

private:
    QString _file_name;
};

#endif // APLREADCONF_H
