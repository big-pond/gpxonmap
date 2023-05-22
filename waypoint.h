#ifndef WAYPOINT_H
#define WAYPOINT_H

#include "trackpoint.h"
#include <QImage>
class WayPoint : public TrackPoint
{
public:
    enum NamePos {BottomRight, Bottom, BottomLeft, Left, TopLeft, Top, TopRight, Right};
    WayPoint(double lat = 0, double lon = 0, double alt = 0, QDateTime tm = QDateTime());
    WayPoint(double lt, double ln, double at, QDate dt, QTime tm);
    WayPoint(const WayPoint &other);
    QString name() {return _name;}
    void setName(const QString& val) {_name = val;}
    QString cmt() {return _cmt;}
    void setCmt(const QString& val) {_cmt = val;}
    QString desc() {return _desc;}
    void setDesc(const QString& val) {_desc = val;}
    QString src() {return _src;}
    void setSrc(const QString& val) {_src = val;}
    QString sym() {return _sym;}
    void setSym(const QString& val) {_sym = val;}
    QString type() {return _type;}
    void setType(const QString& val) {_type = val;}

    void setImage(QImage* val){_image = val;}
    void setImage(QImage* val, int dx, int dy){_image = val, _dx = dx; _dy = dy;}
    QImage* image(){return _image;}

    int dx() {return _dx;}
    void setDx(int val){ _dx = val;}
    int dy() {return _dy;}
    void setDy(int val){ _dy = val;}

    QString marking(){return _marking;}
    void setMarking(const QString& val){_marking = val;}
    bool isMarking(){return !_marking.isEmpty();}

    double fuel(){return _fuel;}
    void setFuel(const double& val){_fuel = val;}
    bool isFuel(){return _fuel!=0;}

    NamePos namepos(){return _namepos;}
    void setNamepos(NamePos val){_namepos = val;}
    bool isNamePos(){return _namepos>0;}

    bool control(){return _control;}
    void setControl(bool val){_control = val;}

    QDateTime arr(){return _arr;}
    void setArr(const QDateTime& val){_arr = val;}
    QDateTime dep(){return _dep;}
    void setDep(const QDateTime& val){_dep = val;}

    WayPoint& operator=(const WayPoint& other);
    bool operator< ( const WayPoint& other ) const;
protected:
    QString _name;
    QString _cmt;
    QString _desc;
    QString _src;
    QString _sym;
    QString _type;

    QImage* _image;
    int _dx, _dy;//смещение значка

//Для использования WayPoint в качестве дополнительной информации для точки трека
//Если путевая точка отдельная (обычная) то marking = isNull() или isEmpty(), а знак записывается в поле sym
//если путевая точка принадлежит треку (мое расширение) то знак выбирается по значению marking a sym=""
    QString _marking; //Возможные значения смотри symdet.cpp
    double _fuel;
    NamePos _namepos;
    bool _control;
    //Для остановок (по ним определяются точки въезда и выезда со стоянок, заправок...
    QDateTime _arr;  //Время прибытия
    QDateTime _dep;  //Время отправления

};
//marking всегда используется для пометки точки трека и никогда!!! для обычных путевых точек
//для расчетов trip используются MRK_CONTROL, MRK_STOPSTART, MRK_STOPEND, MRK_STOPPERSTART,
//MRK_STOPPEREND, MRK_FUELSTART, MRK_FUEL, MRK_FUELEND
#endif // WAYPOINT_H
