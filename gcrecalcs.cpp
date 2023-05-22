#include "gcrecalcs.h"
#include "ellipsoid.h"
#include "gcrecalc.h"
#include "util.h"

GCRecalcs::GCRecalcs()
{
     ellipsoids[E_WGS84] = new Ellipsoid(6378137, 1 / 298.257223563);
     ellipsoids[E_PE90] = new Ellipsoid(6378136, 1 / 298.25784);
     ellipsoids[E_KRASOVSKY] = new Ellipsoid(6378245, 1.0 / 298.3);
     ellipsoids[E_S6371000] = new Ellipsoid(6371000, 0);
     ellipsoids[E_S6378137] = new Ellipsoid(6378173, 0);

     gcrecalcs[SC42_PE90] = new GCRecalc(ellipsoids[E_KRASOVSKY], ellipsoids[E_PE90],
                                         23.557, -140.844, -79.778, -0.0023, -0.34646, -0.79421, -0.228e-6);
     gcrecalcs[PE90_WGS84] = new GCRecalc(ellipsoids[E_PE90], ellipsoids[E_WGS84],
                                          -0.013, 0.106, 0.022, -0.0023, 0.00354, -0.00421, -0.008e-6);
     gcrecalcs[SC42_PE90] = new GCRecalc(ellipsoids[E_KRASOVSKY], ellipsoids[E_PE90],
                                         23.557, -140.844, -79.778, -0.0023, -0.34646, -0.79421, -0.228e-6);
     gcrecalcs[SC42_WGS84] = new GCRecalc(ellipsoids[E_KRASOVSKY], ellipsoids[E_WGS84],
                                          -23.92, +141.27, +80.9, 0, +0.35, +0.82, 0.12);
}

void GCRecalcs::geoRecalc(int id1, int id2, double& lat, double& lon, double& h, bool deg)
{
    if (deg) {
        lat = radians(lat);
        lon = radians(lon);
    }
    if (id1 == S_SC42) {
        if (id2 == S_WGS84) {
            gcrecalcs[SC42_PE90]->blh_A_to_B(lat, lon, h);
            gcrecalcs[PE90_WGS84]->blh_A_to_B(lat, lon, h);
        }
        else if (id2 == S_PE90)
            gcrecalcs[SC42_PE90]->blh_A_to_B(lat, lon, h);
        else if (id2 == S_S6371000) {
            gcrecalcs[SC42_PE90]->blh_A_to_B(lat, lon, h);
            gcrecalcs[PE90_WGS84]->blh_A_to_B(lat, lon, h);
        }
        else if (id2 == S_S6378137) {
            gcrecalcs[SC42_PE90]->blh_A_to_B(lat, lon, h);
            gcrecalcs[PE90_WGS84]->blh_A_to_B(lat, lon, h);
        }
    }
    else if (id1 == S_WGS84) {
        if (id2 == S_SC42) {
            gcrecalcs[PE90_WGS84]->blh_B_to_A(lat, lon, h);
            gcrecalcs[SC42_PE90]->blh_B_to_A(lat, lon, h);
        }
        else if (id2 == S_S6371000) {
        }
        else if (id2 == S_S6378137) {
        }
        else if (id2 == S_PE90)
            gcrecalcs[PE90_WGS84]->blh_B_to_A(lat, lon, h);
    }
    else if (id1 == S_S6371000) {
        if (id2 == S_SC42) {
            gcrecalcs[PE90_WGS84]->blh_B_to_A(lat, lon, h);
            gcrecalcs[SC42_PE90]->blh_B_to_A(lat, lon, h);
        }
        else if (id2 == S_WGS84) {
        }
        else if (id2 == S_S6378137) {
        }
    }
    else if (id1 == S_S6378137) {
        if (id2 == S_SC42) {
            gcrecalcs[PE90_WGS84]->blh_B_to_A(lat, lon, h);
            gcrecalcs[SC42_PE90]->blh_B_to_A(lat, lon, h);
        }
        else if (id2 == S_WGS84) {
            }
        else if (id2 == S_S6371000) {
        }
    }
    else if (id1 == S_PE90) {
        if (id2 == S_SC42)
            gcrecalcs[SC42_PE90]->blh_B_to_A(lat, lon, h);
        else if (id2 == S_WGS84)
            gcrecalcs[PE90_WGS84]->blh_A_to_B(lat, lon, h);
        else if (id2 == S_S6371000)
            gcrecalcs[PE90_WGS84]->blh_A_to_B(lat, lon, h);
        else if (id2 == S_S6378137)
            gcrecalcs[PE90_WGS84]->blh_A_to_B(lat, lon, h);
    }
    if (deg)
    {
        lat = degrees(lat);
        lon = degrees(lon);
    }
}

Ellipsoid *GCRecalcs::getEllipsoid(int key)
{
    return ellipsoids[key];
}

