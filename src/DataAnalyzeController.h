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
    // Row 1
    Q_PROPERTY(bool            visible1         READ visible1         NOTIFY visible1Changed)
    Q_PROPERTY(QStringList     fieldList1       READ fieldList1       NOTIFY fieldList1Changed)
    Q_PROPERTY(QString         scale1           READ scale1           NOTIFY scale1Changed)
    Q_PROPERTY(QString         offsetX1         READ offsetX1         NOTIFY offsetX1Changed)
    Q_PROPERTY(QString         offsetY1         READ offsetY1         NOTIFY offsetY1Changed)
    // Row 2
    Q_PROPERTY(bool            visible2         READ visible2         NOTIFY visible2Changed)
    Q_PROPERTY(QStringList     fieldList2       READ fieldList2       NOTIFY fieldList2Changed)
    Q_PROPERTY(QString         scale2           READ scale2           NOTIFY scale2Changed)
    Q_PROPERTY(QString         offsetX2         READ offsetX2         NOTIFY offsetX2Changed)
    Q_PROPERTY(QString         offsetY2         READ offsetY2         NOTIFY offsetY2Changed)
    // Row 3
    Q_PROPERTY(bool            visible3         READ visible3         NOTIFY visible3Changed)
    Q_PROPERTY(QStringList     fieldList3       READ fieldList3       NOTIFY fieldList3Changed)
    Q_PROPERTY(QString         scale3           READ scale3           NOTIFY scale3Changed)
    Q_PROPERTY(QString         offsetX3         READ offsetX3         NOTIFY offsetX3Changed)
    Q_PROPERTY(QString         offsetY3         READ offsetY3         NOTIFY offsetY3Changed)
    // Row 4
    Q_PROPERTY(bool            visible4         READ visible4         NOTIFY visible4Changed)
    Q_PROPERTY(QStringList     fieldList4       READ fieldList4       NOTIFY fieldList4Changed)
    Q_PROPERTY(QString         scale4           READ scale4           NOTIFY scale4Changed)
    Q_PROPERTY(QString         offsetX4         READ offsetX4         NOTIFY offsetX4Changed)
    Q_PROPERTY(QString         offsetY4         READ offsetY4         NOTIFY offsetY4Changed)
    // Row 5
    Q_PROPERTY(bool            visible5         READ visible5         NOTIFY visible5Changed)
    Q_PROPERTY(QStringList     fieldList5       READ fieldList5       NOTIFY fieldList5Changed)
    Q_PROPERTY(QString         scale5           READ scale5           NOTIFY scale5Changed)
    Q_PROPERTY(QString         offsetX5         READ offsetX5         NOTIFY offsetX5Changed)
    Q_PROPERTY(QString         offsetY5         READ offsetY5         NOTIFY offsetY5Changed)
    // Row 6
    Q_PROPERTY(bool            visible6         READ visible6         NOTIFY visible6Changed)
    Q_PROPERTY(QStringList     fieldList6       READ fieldList6       NOTIFY fieldList6Changed)
    Q_PROPERTY(QString         scale6           READ scale6           NOTIFY scale6Changed)
    Q_PROPERTY(QString         offsetX6         READ offsetX6         NOTIFY offsetX6Changed)
    Q_PROPERTY(QString         offsetY6         READ offsetY6         NOTIFY offsetY6Changed)
    // Row 7
    Q_PROPERTY(bool            visible7         READ visible7         NOTIFY visible7Changed)
    Q_PROPERTY(QStringList     fieldList7       READ fieldList7       NOTIFY fieldList7Changed)
    Q_PROPERTY(QString         scale7           READ scale7           NOTIFY scale7Changed)
    Q_PROPERTY(QString         offsetX7         READ offsetX7         NOTIFY offsetX7Changed)
    Q_PROPERTY(QString         offsetY7         READ offsetY7         NOTIFY offsetY7Changed)
    // Row 8
    Q_PROPERTY(bool            visible8         READ visible8         NOTIFY visible8Changed)
    Q_PROPERTY(QStringList     fieldList8       READ fieldList8       NOTIFY fieldList8Changed)
    Q_PROPERTY(QString         scale8           READ scale8           NOTIFY scale8Changed)
    Q_PROPERTY(QString         offsetX8         READ offsetX8         NOTIFY offsetX8Changed)
    Q_PROPERTY(QString         offsetY8         READ offsetY8         NOTIFY offsetY8Changed)
    // Row 9
    Q_PROPERTY(bool            visible9         READ visible9         NOTIFY visible9Changed)
    Q_PROPERTY(QStringList     fieldList9       READ fieldList9       NOTIFY fieldList9Changed)
    Q_PROPERTY(QString         scale9           READ scale9           NOTIFY scale9Changed)
    Q_PROPERTY(QString         offsetX9         READ offsetX9         NOTIFY offsetX9Changed)
    Q_PROPERTY(QString         offsetY9         READ offsetY9         NOTIFY offsetY9Changed)
    // Row 10
    Q_PROPERTY(bool            visible10        READ visible10        NOTIFY visible10Changed)
    Q_PROPERTY(QStringList     fieldList10      READ fieldList10      NOTIFY fieldList10Changed)
    Q_PROPERTY(QString         scale10          READ scale10          NOTIFY scale10Changed)
    Q_PROPERTY(QString         offsetX10        READ offsetX10        NOTIFY offsetX10Changed)
    Q_PROPERTY(QString         offsetY10        READ offsetY10        NOTIFY offsetY9Changed)

    // Row 1
    Q_INVOKABLE void setFieldList1 (QString table);
    Q_INVOKABLE void setField1     (QString field);
    Q_INVOKABLE void setScale1     (QString scale);
    Q_INVOKABLE void setOffsetX1   (QString offset);
    Q_INVOKABLE void setOffsetY1   (QString offset);
    Q_INVOKABLE void setVisible1   (bool visible);
    Q_INVOKABLE void setLineStyle1 (int style);
    Q_INVOKABLE void setLineColor1 (int color);
    // Row 2
    Q_INVOKABLE void setFieldList2 (QString table);
    Q_INVOKABLE void setField2     (QString field);
    Q_INVOKABLE void setScale2     (QString scale);
    Q_INVOKABLE void setOffsetX2   (QString offset);
    Q_INVOKABLE void setOffsetY2   (QString offset);
    Q_INVOKABLE void setVisible2   (bool visible);
    Q_INVOKABLE void setLineStyle2 (int style);
    Q_INVOKABLE void setLineColor2 (int color);
    // Row 3
    Q_INVOKABLE void setFieldList3 (QString table);
    Q_INVOKABLE void setField3     (QString field);
    Q_INVOKABLE void setScale3     (QString scale);
    Q_INVOKABLE void setOffsetX3   (QString offset);
    Q_INVOKABLE void setOffsetY3   (QString offset);
    Q_INVOKABLE void setVisible3   (bool visible);
    Q_INVOKABLE void setLineStyle3 (int style);
    Q_INVOKABLE void setLineColor3 (int color);
    // Row 4
    Q_INVOKABLE void setFieldList4 (QString table);
    Q_INVOKABLE void setField4     (QString field);
    Q_INVOKABLE void setScale4     (QString scale);
    Q_INVOKABLE void setOffsetX4   (QString offset);
    Q_INVOKABLE void setOffsetY4   (QString offset);
    Q_INVOKABLE void setVisible4   (bool visible);
    Q_INVOKABLE void setLineStyle4 (int style);
    Q_INVOKABLE void setLineColor4 (int color);
    // Row 5
    Q_INVOKABLE void setFieldList5 (QString table);
    Q_INVOKABLE void setField5     (QString field);
    Q_INVOKABLE void setScale5     (QString scale);
    Q_INVOKABLE void setOffsetX5   (QString offset);
    Q_INVOKABLE void setOffsetY5   (QString offset);
    Q_INVOKABLE void setVisible5   (bool visible);
    Q_INVOKABLE void setLineStyle5 (int style);
    Q_INVOKABLE void setLineColor5 (int color);
    // Row 6
    Q_INVOKABLE void setFieldList6 (QString table);
    Q_INVOKABLE void setField6     (QString field);
    Q_INVOKABLE void setScale6     (QString scale);
    Q_INVOKABLE void setOffsetX6   (QString offset);
    Q_INVOKABLE void setOffsetY6   (QString offset);
    Q_INVOKABLE void setVisible6   (bool visible);
    Q_INVOKABLE void setLineStyle6 (int style);
    Q_INVOKABLE void setLineColor6 (int color);
    // Row 7
    Q_INVOKABLE void setFieldList7 (QString table);
    Q_INVOKABLE void setField7     (QString field);
    Q_INVOKABLE void setScale7     (QString scale);
    Q_INVOKABLE void setOffsetX7   (QString offset);
    Q_INVOKABLE void setOffsetY7   (QString offset);
    Q_INVOKABLE void setVisible7   (bool visible);
    Q_INVOKABLE void setLineStyle7 (int style);
    Q_INVOKABLE void setLineColor7 (int color);
    // Row 8
    Q_INVOKABLE void setFieldList8 (QString table);
    Q_INVOKABLE void setField8     (QString field);
    Q_INVOKABLE void setScale8     (QString scale);
    Q_INVOKABLE void setOffsetX8   (QString offset);
    Q_INVOKABLE void setOffsetY8   (QString offset);
    Q_INVOKABLE void setVisible8   (bool visible);
    Q_INVOKABLE void setLineStyle8 (int style);
    Q_INVOKABLE void setLineColor8 (int color);
    // Row 9
    Q_INVOKABLE void setFieldList9 (QString table);
    Q_INVOKABLE void setField9     (QString field);
    Q_INVOKABLE void setScale9     (QString scale);
    Q_INVOKABLE void setOffsetX9   (QString offset);
    Q_INVOKABLE void setOffsetY9   (QString offset);
    Q_INVOKABLE void setVisible9   (bool visible);
    Q_INVOKABLE void setLineStyle9 (int style);
    Q_INVOKABLE void setLineColor9 (int color);
    // Row 10
    Q_INVOKABLE void setFieldList10 (QString table);
    Q_INVOKABLE void setField10     (QString field);
    Q_INVOKABLE void setScale10     (QString scale);
    Q_INVOKABLE void setOffsetX10   (QString offset);
    Q_INVOKABLE void setOffsetY10   (QString offset);
    Q_INVOKABLE void setVisible10   (bool visible);
    Q_INVOKABLE void setLineStyle10 (int style);
    Q_INVOKABLE void setLineColor10 (int color);

    QStringList      tableList              () { return _tableList; }
    QStringList      lineList               () { return _lineList; }
    QStringList      colorList              () { return _colorList; }
    // Row 1
    QStringList      fieldList1             () { return _fieldList[0]; }
    QString          scale1                 () { return QString::number(_scale[0], 'f', 3); }
    QString          offsetX1               () { return QString::number(_offsetX[0]); }
    QString          offsetY1               () { return QString::number(_offsetY[0], 'f', 2); }
    bool             visible1               () { return _visible[0]; }
    // Row 2
    QStringList      fieldList2             () { return _fieldList[1]; }
    QString          scale2                 () { return QString::number(_scale[1], 'f', 3); }
    QString          offsetX2               () { return QString::number(_offsetX[1]); }
    QString          offsetY2               () { return QString::number(_offsetY[1], 'f', 2); }
    bool             visible2               () { return _visible[1]; }
    // Row 3
    QStringList      fieldList3             () { return _fieldList[2]; }
    QString          scale3                 () { return QString::number(_scale[2], 'f', 3); }
    QString          offsetX3               () { return QString::number(_offsetX[2]); }
    QString          offsetY3               () { return QString::number(_offsetY[2], 'f', 2); }
    bool             visible3               () { return _visible[2]; }
    // Row 4
    QStringList      fieldList4             () { return _fieldList[3]; }
    QString          scale4                 () { return QString::number(_scale[3], 'f', 3); }
    QString          offsetX4               () { return QString::number(_offsetX[3]); }
    QString          offsetY4               () { return QString::number(_offsetY[3], 'f', 2); }
    bool             visible4               () { return _visible[3]; }
    // Row 5
    QStringList      fieldList5             () { return _fieldList[4]; }
    QString          scale5                 () { return QString::number(_scale[4], 'f', 3); }
    QString          offsetX5               () { return QString::number(_offsetX[4]); }
    QString          offsetY5               () { return QString::number(_offsetY[4], 'f', 2); }
    bool             visible5               () { return _visible[4]; }
    // Row 6
    QStringList      fieldList6             () { return _fieldList[5]; }
    QString          scale6                 () { return QString::number(_scale[5], 'f', 3); }
    QString          offsetX6               () { return QString::number(_offsetX[5]); }
    QString          offsetY6               () { return QString::number(_offsetY[5], 'f', 2); }
    bool             visible6               () { return _visible[5]; }
    // Row 7
    QStringList      fieldList7             () { return _fieldList[6]; }
    QString          scale7                 () { return QString::number(_scale[6], 'f', 3); }
    QString          offsetX7               () { return QString::number(_offsetX[6]); }
    QString          offsetY7               () { return QString::number(_offsetY[6], 'f', 2); }
    bool             visible7               () { return _visible[6]; }
    // Row 8
    QStringList      fieldList8             () { return _fieldList[7]; }
    QString          scale8                 () { return QString::number(_scale[7], 'f', 3); }
    QString          offsetX8               () { return QString::number(_offsetX[7]); }
    QString          offsetY8               () { return QString::number(_offsetY[7], 'f', 2); }
    bool             visible8               () { return _visible[7]; }
    // Row 9
    QStringList      fieldList9             () { return _fieldList[8]; }
    QString          scale9                 () { return QString::number(_scale[8], 'f', 3); }
    QString          offsetX9               () { return QString::number(_offsetX[8]); }
    QString          offsetY9               () { return QString::number(_offsetY[8], 'f', 2); }
    bool             visible9               () { return _visible[8]; }
    // Row 10
    QStringList      fieldList10            () { return _fieldList[9]; }
    QString          scale10                () { return QString::number(_scale[9], 'f', 3); }
    QString          offsetX10              () { return QString::number(_offsetX[9]); }
    QString          offsetY10              () { return QString::number(_offsetY[9], 'f', 2); }
    bool             visible10              () { return _visible[9]; }

signals:
    void tableListChanged       ();
    void lineListChanged        ();
    void colorListChanged       ();
    // Row 1
    void fieldList1Changed      ();
    void scale1Changed          ();
    void offsetX1Changed        ();
    void offsetY1Changed        ();
    void visible1Changed        ();
    // Row 2
    void fieldList2Changed      ();
    void scale2Changed          ();
    void offsetX2Changed        ();
    void offsetY2Changed        ();
    void visible2Changed        ();
    // Row 3
    void fieldList3Changed      ();
    void scale3Changed          ();
    void offsetX3Changed        ();
    void offsetY3Changed        ();
    void visible3Changed        ();
    // Row 4
    void fieldList4Changed      ();
    void scale4Changed          ();
    void offsetX4Changed        ();
    void offsetY4Changed        ();
    void visible4Changed        ();
    // Row 5
    void fieldList5Changed      ();
    void scale5Changed          ();
    void offsetX5Changed        ();
    void offsetY5Changed        ();
    void visible5Changed        ();
    // Row 6
    void fieldList6Changed      ();
    void scale6Changed          ();
    void offsetX6Changed        ();
    void offsetY6Changed        ();
    void visible6Changed        ();
    // Row 7
    void fieldList7Changed      ();
    void scale7Changed          ();
    void offsetX7Changed        ();
    void offsetY7Changed        ();
    void visible7Changed        ();
    // Row 8
    void fieldList8Changed      ();
    void scale8Changed          ();
    void offsetX8Changed        ();
    void offsetY8Changed        ();
    void visible8Changed        ();
    // Row 9
    void fieldList9Changed      ();
    void scale9Changed          ();
    void offsetX9Changed        ();
    void offsetY9Changed        ();
    void visible9Changed        ();
    // Row 10
    void fieldList10Changed     ();
    void scale10Changed         ();
    void offsetX10Changed       ();
    void offsetY10Changed       ();
    void visible10Changed       ();

    void plotGraph              (QString tables,
                                 QString fields,
                                 int     offsetX,
                                 float   offsetY,
                                 float   scale,
                                 int     linestyle,
                                 int     color,
                                 bool    visible,
                                 bool    from);             // false:DataAnalyzeController,true:Other

private slots:
    void                _setTableList(QString table);

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
    QString                                tables[MAX_LINE_NUM];
    QString                                fields[MAX_LINE_NUM];
};

#endif // DATAANALYZECONTROLLER_H
