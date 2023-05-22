#ifndef GEOPOINT_H
#define GEOPOINT_H
#include <QString>

class GeoPoint
{
public:
    static double R; //Earth radus (sphere)

    static const QChar CDEG;
    static const QChar CMIN;
    static const QChar CSEC;
    static const QChar CPNT;

    static const QChar CNORTH;
    static const QChar CSOUTH;
    static const QChar CWEST;
    static const QChar CEAST;

    static const int DPR; //precision for deg
    static const int DFW; //field width for deg
    static const int MPR; //precision for min
    static const int MFW; //field width for min
    static const int SPR; //precision for sec
    static const int SFW; //field width for sec

    //Format of the geographical coordinates
    enum DispFormat {Deg, DegMin, DegMinSec, DegFloat, Rad, RadFloat, Metre, Pixel};
    enum CoordType {Latitude, Longitude};

    static DispFormat display_format;

    GeoPoint(double lat = 0, double lon = 0, double alt = 0) : latitude(lat), longitude(lon), altitude(alt) {}
    GeoPoint(const GeoPoint& other): latitude(other.latitude), longitude(other.longitude), altitude(other.altitude){}

    GeoPoint& operator=(const GeoPoint& other);
    bool operator==( const GeoPoint& other ) const;
    bool operator!=( const GeoPoint& other ) const;
    bool operator < ( const GeoPoint& other ) const;

    double sphDistanceTo(const GeoPoint &other) const;
    double sphAzimuthTo(const GeoPoint &other) const;
    GeoPoint atSphDistanceAndAzimuth(const double &dist, const double &az, const double &alt=0);

    double latitude;  //negative for south hemisphere
    double longitude; //negative for west  hemisphere
    double altitude;  //altitude

    /*-------------------------Static functions--------------------------------------------*/
    static void degToDms(const double &deg, double &d, double &m, double &s, int &sign);
    static double dmsToDeg(const double &d, const double &m, const double &s, const int sign);
    static void degToDm(const double &deg, double &d, double &m, int &sign);
    static double dmToDeg(const double &d, const double &m, const int sign);

    static QString latToDmsStr(const double &lat);
    static QString latToDmStr(const double &lat);
    static QString latToDegStr(const double &lat);
    static QString latToFloatStr(const double &lat);
    static QString latToRadStr(const double &lat);
    static QString latToRadFloatStr(const double &lat);
    static QString latToStr(const double &lat, DispFormat format);

    static QString lonToDmsStr(const double &lon);
    static QString lonToDmStr(const double &lon);
    static QString lonToDegStr(const double &lon);
    static QString lonToFloatStr(const double &lon);
    static QString lonToRadStr(const double &lon);
    static QString lonToRadFloatStr(const double &lon);
    static QString lonToStr(const double &lon, DispFormat format);

    static QString degToStr(const double& val, CoordType type, DispFormat format);
    static QString getFormat(DispFormat format);
};

#endif // GEOPOINT_H
