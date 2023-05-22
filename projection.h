#ifndef PROJECTION_H
#define PROJECTION_H

#include <QString>

class Ellipsoid;

class Projection
{
public:
    enum Id {UNKNOWN, MERCATOR, GK, UTM};
    enum {MERC_WGS84 = 1,  MERC_S6371000 = 2, MERC_S6378137 = 3, GK_KRASOVSKY = 4};

    explicit Projection(const QString& el_name, Ellipsoid* el);
    static Projection* createProection(const Projection::Id& _id, Ellipsoid* el);
    Ellipsoid* getEllipsoid(){return ellipsoid;}
    //    const QString& getName(){return name;}
    virtual void geo_to_metre(double& x, double& y, double lat, double lon, bool deg=false, int zone=0) = 0;
    virtual void metre_to_geo(double x, double y, double& lat, double& lon, bool deg=false, int zone=0) = 0;

    virtual void tile_to_degree(const int& x, const int& y, const int& z, double& lat, double& lon)=0;
    virtual void degree_to_tile(int& x, int& y, const int& z, const double& lat, const double& lon)=0;
    //Нумерация зоны от долготы -180° (Например Екатеринбург в 41 зоне) соответствут зонам UTM
    int getZone(const double& lon) { return ((int)((lon+180.)/6.)) + 1; }

    double getDistanceOnPlane(const double& x1, const double& y1,const double& x2,const double& y2);

protected:
    Ellipsoid *ellipsoid;
    QString name;
    QString el_name;
    double a;//большая полуось эллипсоида
    double e;//эксцентриситет
    double k0;
    double b; //малая полуось эллипсоида
    double e2, e4, e6;
    double e_2;
    double n;
};
/**** Пустая проекция (возвращает координаты без преобразования)****/
class EmptyPro: public Projection
{
public:
    explicit EmptyPro(const QString& el_name, Ellipsoid* el);

    virtual void geo_to_metre(double& x, double& y, double lat, double lon, bool deg=false, int zone=0);
    virtual void metre_to_geo(double x, double y, double& lat, double& lon, bool deg=false, int zone=0);

    virtual void tile_to_degree(const int& /*x*/, const int& /*y*/, const int& /*z*/,
                                double& lat, double& lon){lat = 0; lon = 0;}
    virtual void degree_to_tile(int& x, int& y, const int& /*z*/,
                                const double& /*lat*/, const double& /*lon*/){x = 0; y = 0;}
};

/**** проекция Меркатора ****/
class MercatorPro :public Projection
{
public:
    explicit MercatorPro(const QString& el_name, Ellipsoid* el);

    virtual void geo_to_metre(double& x, double& y, double lat, double lon, bool deg=false, int zone=0);
    virtual void metre_to_geo(double x, double y, double& lat, double& lon, bool deg=false, int zone=0);

    double iterLat(const double& y,const int& z,const double& b);
    virtual void tile_to_degree(const int& x, const int& y, const int& z,
                                double& lat, double& lon);
    virtual void degree_to_tile(int& x, int& y, const int& z,
                                const double& lat, const double& lon);
private:
    double iterBr(const double& x, const double& brn);
};
/**** проекция Гаусса-Крюгера ****/
class GKPro : public Projection
{
public:
    explicit GKPro(const QString& el_name, Ellipsoid* el);

    virtual void geo_to_metre(double& x, double& y, double lat, double lon, bool deg=false, int zone=0);
    virtual void metre_to_geo(double x, double y, double& lat, double& lon, bool deg=false, int zone=0);

    virtual void tile_to_degree(const int& /*x*/, const int& /*y*/, const int& /*z*/,
                                double& lat, double& lon){lat = 0; lon = 0;}
    virtual void degree_to_tile(int& x, int& y, const int& /*z*/,
                                const double& /*lat*/, const double& /*lon*/){x = 0; y = 0;}
};

/**** проекция UTM ****/
class UTMPro : public Projection
{
public:
    explicit UTMPro(const QString& el_name, Ellipsoid* el);

    virtual void geo_to_metre(double& x, double& y, double lat, double lon, bool deg=false, int zone=0);
    virtual void metre_to_geo(double x, double y, double& lat, double& lon, bool deg=false, int zone=0);

    virtual void tile_to_degree(const int& /*x*/, const int& /*y*/, const int& /*z*/,
                                double& lat, double& lon){lat = 0; lon = 0;}
    virtual void degree_to_tile(int& x, int& y, const int& /*z*/,
                                const double& /*lat*/, const double& /*lon*/){x = 0; y = 0;}
};

#endif // PROJECTION_H
