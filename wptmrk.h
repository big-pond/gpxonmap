#ifndef WPTMRK_H
#define SYMDET_H

#include <QString>
#include <QList>
#include <QImage>
#include <QAbstractTableModel>

class WayPoint;

class WptMrk : public QAbstractTableModel
{
    Q_OBJECT
public:
    //Маркировка marking
    static const QString ARR;
    static const QString BED;
    static const QString CAM;
    static const QString CONTROL;
    static const QString DEP;
    static const QString FINISH;
    static const QString FOOD;
    static const QString FUEL;
    static const QString GASSTATION;
    static const QString HMAX;
    static const QString HMIN;
    static const QString LABEL;
    static const QString POINT;
    static const QString POLICE;
    static const QString SPEEDMAX;
    static const QString START;
    static const QString STOP;
    static const QString TANKFUEL;
    static const QString TRAFLIGHT;
//Положение подписей
    static const QString TOP;
    static const QString TOPRIGHT;
    static const QString RIGHT;
    static const QString BOTTOMRIGHT;
    static const QString BOTTOM;
    static const QString BOTTOMLEFT;
    static const QString LEFT;
    static const QString TOPLEFT;

    struct SymRec
    {
        QString marking;
        QString name;
        int dx;
        int dy;
        QImage image;
        QString imagepath;
        SymRec() : marking(), name(), dx(0), dy(0), imagepath(){}
        SymRec(QString marking_, QString name_, int dx_, int dy_, QString impth)
            :marking(marking_), name(name_), dx(dx_), dy(dy_), imagepath(impth)
        {}
    };
    explicit WptMrk(QObject *parent=0);
    int	rowCount(const QModelIndex& parent = QModelIndex()) const;
    int	columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role/* = Qt::DisplayRole*/) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void readData(const QString& fname);
    void loadImages();
    QString getMarking(int i) {return det[i].marking;}
    QString getName(int i) {return det[i].name;}

    QImage* getImage(int i){return &det[i].image;}
    QImage* getImage(const QString mark);
    int getDx(int i){return det[i].dx;}
    int getDy(int i){return det[i].dy;}
    QString getImagePath(const QString mark);
    QString getImagePath(int i){return det[i].imagepath;}
    int markingIndex(const QString& mark);
    bool isMarking(const QString& mark);

    void setImageToWayPoint(WayPoint* wpt);
    static QString getNamePos(WayPoint* wpt);
    static void setNamePos(WayPoint* wpt, QString np);

protected:
    QList<SymRec> det;
    int colcount;

    void setDefault();
};

#endif // WPTMRK_H
