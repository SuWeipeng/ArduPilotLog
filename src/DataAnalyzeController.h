#ifndef DATAANALYZECONTROLLER_H
#define DATAANALYZECONTROLLER_H

#include <QObject>
#include "APLLoggingCategory.h"
#include "qcustomplot.h"

Q_DECLARE_LOGGING_CATEGORY(DATA_ANALYZE_LOG)

#define MAX_LINE_NUM 10

class DataAnalyzeController : public QObject
{
    Q_OBJECT
public:
    DataAnalyzeController();

    Q_PROPERTY(QStringList     tableList        READ tableList        NOTIFY tableListChanged)
    Q_PROPERTY(QStringList     lineList         READ lineList         NOTIFY lineListChanged)
    Q_PROPERTY(QStringList     colorList        READ colorList        NOTIFY colorListChanged)
    Q_PROPERTY(bool            visible1         READ visible1         NOTIFY visible1Changed)
    Q_PROPERTY(QStringList     fieldList1       READ fieldList1       NOTIFY fieldList1Changed)
    Q_PROPERTY(QString         scale1           READ scale1           NOTIFY scale1Changed)
    Q_PROPERTY(QString         offsetX1         READ offsetX1         NOTIFY offsetX1Changed)
    Q_PROPERTY(QString         offsetY1         READ offsetY1         NOTIFY offsetY1Changed)


    Q_INVOKABLE void setFieldList1 (QString table);
    Q_INVOKABLE void setField1     (QString field);
    Q_INVOKABLE void setScale1     (QString scale);
    Q_INVOKABLE void setOffsetX1   (QString offset);
    Q_INVOKABLE void setOffsetY1   (QString offset);
    Q_INVOKABLE void setVisible1   (bool visible);
    Q_INVOKABLE void setLineStyle1 (int style);
    Q_INVOKABLE void setLineColor1 (int color);

    QStringList      tableList              () { return _tableList; }
    QStringList      lineList               () { return _lineList; }
    QStringList      colorList              () { return _colorList; }
    QStringList      fieldList1             () { return _fieldList[0]; }
    QString          scale1                 () { return QString::number(_scale[0], 'f', 3); }
    QString          offsetX1               () { return QString::number(_offsetX[0]); }
    QString          offsetY1               () { return QString::number(_offsetY[0], 'f', 2); }
    bool             visible1               () { return _visible[0]; }

signals:
    void tableListChanged       ();
    void lineListChanged        ();
    void colorListChanged       ();
    void fieldList1Changed      ();
    void scale1Changed          ();
    void offsetX1Changed        ();
    void offsetY1Changed        ();
    void visible1Changed        ();

private slots:
    void _setTableList(QString table);

private:
    bool                _isNumber(QString n);
    void                _plot();
    void                _lineStyle(int index, int i);
    bool                _visible[MAX_LINE_NUM];
    QStringList         _tableList;
    QStringList         _lineList;
    QStringList         _colorList;
    QStringList         _fieldList[MAX_LINE_NUM];
    float               _scale[MAX_LINE_NUM];
    int                 _offsetX[MAX_LINE_NUM];
    float               _offsetY[MAX_LINE_NUM];
    int                 _style[MAX_LINE_NUM];
    int                 _color[MAX_LINE_NUM];

public:
    QVector<QCPScatterStyle::ScatterShape> shapes[MAX_LINE_NUM];
    QVector<QColor>                        colors[MAX_LINE_NUM];
    QString                                tables[MAX_LINE_NUM];
    QString                                fields[MAX_LINE_NUM];
};

#endif // DATAANALYZECONTROLLER_H
