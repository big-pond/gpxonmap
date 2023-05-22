#include "geopoint.h"
#include <cmath>
#include "util.h"

#include <QDebug>

double GeoPoint::R = 6371007.2;

const QChar GeoPoint::CDEG(0x00B0); //degree sign - °
const QChar GeoPoint::CMIN(0x0027); // minute sign - '
const QChar GeoPoint::CSEC(0x0022); // second sign - "
const QChar GeoPoint::CPNT('.');

const QChar GeoPoint::CNORTH('N');
const QChar GeoPoint::CSOUTH('S');
const QChar GeoPoint::CWEST('W');
const QChar GeoPoint::CEAST('E');

const int GeoPoint::DPR = 6;
const int GeoPoint::DFW = GeoPoint::DPR+3;
const int GeoPoint::MPR = 5;
const int GeoPoint::MFW = GeoPoint::MPR+3;
const int GeoPoint::SPR = 2;
const int GeoPoint::SFW = GeoPoint::SPR+3;

GeoPoint::DispFormat GeoPoint::display_format = GeoPoint::Deg;

GeoPoint &GeoPoint::operator=(const GeoPoint &other)
{
    latitude = other.latitude;
    longitude = other.longitude;
    altitude = other.altitude;
    return *this;
}

bool GeoPoint::operator==(const GeoPoint &other) const
{
    return (latitude == other.latitude) && (longitude == other.longitude) && (altitude == other.altitude);
}

bool GeoPoint::operator!=(const GeoPoint &other) const
{
    return (latitude != other.latitude) || (longitude != other.longitude) || (altitude != other.altitude);
}

bool GeoPoint::operator <(const GeoPoint &other) const
{
    return latitude < other.latitude;
}

double GeoPoint::sphDistanceTo(const GeoPoint& other) const
{
    double lat1 = radians(latitude);
    double lon1 = radians(longitude);
    double lat2 = radians(other.latitude);
    double lon2 = radians(other.longitude);
    double cosfi1 = cos(lat1);
    double sinfi1 = sin(lat1);
    double cosfi2 = cos(lat2);
    double sinfi2 = sin(lat2);
    double cosdla = cos(lon2-lon1);
    double sindla = sin(lon2-lon1);
    double a = cosfi2*sindla;
    double b = cosfi1*sinfi2-sinfi1*cosfi2*cosdla;
    double dsigma = atan2(sqrt(a*a+b*b),sinfi1*sinfi2+cosfi1*cosfi2*cosdla);
    return dsigma*R; //для совпадения с QGeoCoordinate R  = 6371007.2
}

double GeoPoint::sphAzimuthTo(const GeoPoint& other) const
{
    double lat1 = radians(latitude);
    double lon1 = radians(longitude);
    double lat2 = radians(other.latitude);
    double lon2 = radians(other.longitude);
    double cosfi2 = cos(lat2);
    return degrees(atan2(sin(lon2-lon1)*cosfi2, cos(lat1)*sin(lat2) - sin(lat1)*cosfi2*cos(lon2-lon1)));
}

GeoPoint GeoPoint::atSphDistanceAndAzimuth(const double& dist, const double& az, const double& alt)
{
    double lat1 = radians(latitude);
    double lon1 = radians(longitude);
    double azi1 = radians(az);
    double sigma = dist/R;
    double sinfi1 = sin(lat1);
    double cosfi1 = cos(lat1);
    double cossig = cos(sigma);
    double sinsig = sin(sigma);
    double cosaz1 = cos(azi1);
    double sinaz1 = sin(azi1);

    double lat2 = asin(sinfi1*cossig +cosfi1*sinsig*cosaz1);

    double dlon = atan2(sinsig*sinaz1, cossig*cosfi1-sinsig*sinfi1*cosaz1);
    double lon2 = lon1+dlon;

    return GeoPoint(degrees(lat2), degrees(lon2), alt);
}

void GeoPoint::degToDms(const double &deg, double& d, double& m, double& s, int& sign)
{
    double adeg = fabs(deg);
    d = floor(adeg);
    if (deg >= 0)
        sign = 1;
    else
        sign = -1;
    double mf = (adeg - d) * 60;
    m = floor(mf);
    s = (mf - m) * 60;
}


double GeoPoint::dmsToDeg(const double &d, const double &m, const double &s, const int sign)
{
    double deg = d + m * 60 + s * 3600;
    if (sign < 0)
        deg = -deg;
    return deg;
}

void GeoPoint::degToDm(const double &deg, double& d, double& m, int& sign)
{
    double adeg = fabs(deg);
    d = floor(adeg);
    if (deg >= 0)
        sign = 1;
    else
        sign = -1;
    m = (adeg - d) * 60;
}

double GeoPoint::dmToDeg(const double &d, const double &m, const int sign)
{
    double deg = d + m * 60;
    if (sign < 0)
        deg = -deg;
    return deg;
}

QString GeoPoint::latToDmsStr(const double& lat)
{
    double  d, m, s;
    int sign;
    degToDms(lat, d, m, s, sign);
    QChar hs = CNORTH;
    if (sign == -1)
        hs = CSOUTH;
    return QString("%1°%2'%3\"%4").arg(d,2,'f',0).arg(m,2,'f',0).arg(s,SFW,'f',SPR).arg(hs);
}

QString GeoPoint::latToDmStr(const double& lat)
{
    double d, m;
    int sign;
    degToDm(lat, d, m, sign);
    QChar hs = CNORTH;
    if (sign == -1)
        hs = CSOUTH;
    return QString("%1°%2'%3").arg(d,2,'f',0).arg(m,MFW,'f',MPR).arg(hs);
}

QString GeoPoint::latToDegStr(const double& lat)
{
    double d = fabs(lat);
    QChar hs = CNORTH;
    if (lat < 0)
        hs = CSOUTH;
    return QString("%1°%2").arg(d,0,'f',GeoPoint::DPR).arg(hs);
}

QString GeoPoint::latToFloatStr(const double& lat)
{
    return QString("%1").arg(lat,0,'f',GeoPoint::DPR);
}

QString GeoPoint::latToRadStr(const double& lat)
{
    double rad = fabs(radians(lat));
    QChar hs = CNORTH;
    if (lat < 0)
        hs = CSOUTH;
    return QString("%1%2").arg(rad,0,'f',7).arg(hs);
}

QString GeoPoint::latToRadFloatStr(const double& lat)
{
    double rad = fabs(radians(lat));
    return QString("%1").arg(rad,0,'f',7);
}

QString GeoPoint::latToStr(const double& lat, DispFormat format)
{
    if (format == Deg)
        return latToDegStr(lat);
    else if (format == DegMin)
        return latToDmStr(lat);
    else if (format == DegMinSec)
        return latToDmsStr(lat);
    else if (format == DegFloat)
        return latToFloatStr(lat);
    else if (format == Rad)
        return latToRadStr(lat);
    else // if (format == RadFloat)
        return latToRadFloatStr(lat);
}


QString GeoPoint::lonToDmsStr(const double& lon)
{
    double  d, m, s;
    int sign;
    degToDms(lon, d, m, s, sign);
    QChar hs = CEAST;
    if (sign == -1)
        hs = CWEST;
    return QString("%1°%2'%3\"%4").arg(d,2,'f',0).arg(m,2,'f',0).arg(s,SFW,'f',SPR).arg(hs);
}

QString GeoPoint::lonToDmStr(const double& lon)
{
    double d, m;
    int sign;
    degToDm(lon, d, m, sign);
    QChar hs = CEAST;
    if (sign == -1)
        hs = CWEST;
    return QString("%1°%2'%3").arg(d,2,'f',0).arg(m,MFW,'f',MPR).arg(hs);
}

QString GeoPoint::lonToDegStr(const double& lon)
{
    double d = fabs(lon);
    QChar hs = CEAST;
    if (lon < 0)
        hs = CWEST;
    return QString("%1°%2").arg(d,0,'f',GeoPoint::DPR).arg(hs);
}

QString GeoPoint::lonToFloatStr(const double& lon)
{
    return QString("%1").arg(lon,0,'f',GeoPoint::DPR);
}

QString GeoPoint::lonToRadStr(const double& lon)
{
    double rad = fabs(radians(lon));
    QChar hs = CEAST;
    if (lon < 0)
        hs = CWEST;
    return QString("%1%2").arg(rad,0,'f',7).arg(hs);
}

QString GeoPoint::lonToRadFloatStr(const double& lon)
{
    double rad = fabs(radians(lon));
    return QString("%1").arg(rad,0,'f',7);
}

QString GeoPoint:: lonToStr(const double& lon, DispFormat format)
{
    if (format == Deg)
        return lonToDegStr(lon);
    else if (format == DegMin)
        return lonToDmStr(lon);
    else if (format == DegMinSec)
        return lonToDmsStr(lon);
    else if (format == DegFloat)
        return lonToFloatStr(lon);
    else if (format == Rad)
        return lonToRadStr(lon);
    else // format == RadFloat
        return lonToRadFloatStr(lon);
}

QString GeoPoint::degToStr(const double& val, CoordType type, DispFormat format)
{
    if (type == Latitude)
        return latToStr(val, format);
    else // type == Longitude
        return lonToStr(val, format);
}

QString GeoPoint::getFormat(DispFormat format)
{
    QString sfc;
    switch(format)
    {
    case Deg: sfc = "0.000000°"; break;
    case DegMin: sfc = "00°00.0000'"; break;
    case DegMinSec: sfc = "00°00'00.00\""; break;
    case DegFloat: sfc = "0.000000"; break;
    default: sfc = "0.000000"; break;
    }
    return sfc;
}
