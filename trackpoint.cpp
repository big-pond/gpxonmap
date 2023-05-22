#include "trackpoint.h"

TrackPoint&  TrackPoint::operator=(const TrackPoint &other)
{
    GeoPoint::operator=(other);
    time = other.time;
    return *this;
}

bool TrackPoint::operator==( const TrackPoint &other ) const
{
    return GeoPoint::operator==(other) && (time==other.time);
}

bool TrackPoint::operator!=( const TrackPoint &other ) const
{
    return GeoPoint::operator!=(other) || (time==other.time);
}

bool TrackPoint::operator<( const TrackPoint &other ) const
{
    return time<other.time;
}

