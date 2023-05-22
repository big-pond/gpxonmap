#ifndef GCRECALCS_H
#define GCRECALCS_H

#include <QMap>

class Ellipsoid;
class GCRecalc;

class GCRecalcs
{
public:
    enum {E_WGS84 = 1, E_KRASOVSKY = 2, E_PE90 = 3, E_S6371000 = 4, E_S6378137 = 5}; //Эллипсоиды


    enum {S_WGS84 = 1,  S_PE90 = 2, S_SC42 = 3, S_S6371000 = 4, S_S6378137 = 5}; //Системы геодезических координат
    enum {SC42_PE90 = 1,  PE90_WGS84 = 2,  SC42_WGS84 = 3};
    GCRecalcs();
    void geoRecalc(int id1, int id2, double& lat, double& lon, double& h, bool deg = false);
    Ellipsoid* getEllipsoid(int key);
private:
    QMap<int, Ellipsoid*> ellipsoids;
    QMap<int, GCRecalc*> gcrecalcs;
};

#endif // GCRECALCS_H
