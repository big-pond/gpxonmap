#ifndef TRACKPOINT_H
#define TRACKPOINT_H
#include <QDateTime>
#include "geopoint.h"

class TrackPoint : public GeoPoint
{
public:
    TrackPoint(double lat = 0, double lon = 0, double alt = 0, QDateTime tm = QDateTime()) : GeoPoint(lat, lon, alt), time(tm) {}
    TrackPoint(const TrackPoint &other) : GeoPoint(other), time(other.time){}

    TrackPoint& operator=(const TrackPoint& other);
    bool operator==( const TrackPoint& other ) const;
    bool operator!=( const TrackPoint& other ) const;
    bool operator < ( const TrackPoint& other ) const;
    QDateTime time;
};

#endif // TRACKPOINT_H
