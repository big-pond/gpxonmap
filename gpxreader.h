#ifndef GPXREADER_H
#define GPXREADER_H

#include <QObject>
#include <QDateTime>

//class Track;
class QXmlStreamReader;
class QXmlStreamWriter;
class QIODevice;
class Gpx;
class WayPoint;
class WptMrk;

class GpxReader : public QObject
{
    Q_OBJECT
public:
    static const QLatin1String GPX;

    static const QLatin1String METADATA;

    static const QLatin1String TRK;
    static const QLatin1String WPT;
    static const QLatin1String BOUNDS;
    static const QLatin1String MINLAT;
    static const QLatin1String MINLON;
    static const QLatin1String MAXLAT;
    static const QLatin1String MAXLON;
    static const QLatin1String NAME;
    static const QLatin1String EXTENSIONS;
    static const QLatin1String TRKSEG;
    static const QLatin1String TRACKEXTENSION;
    static const QLatin1String DISPLAYCOLOR;
    static const QLatin1String TRKPT;
    static const QLatin1String LAT;
    static const QLatin1String LON;
    static const QLatin1String ELE;
    static const QLatin1String TIME;
    static const QLatin1String DESC;
    static const QLatin1String CMT;
    static const QLatin1String SRC;
    static const QLatin1String SYM;
    static const QLatin1String TYPE;

    explicit GpxReader(QObject * parent = 0);
    ~GpxReader();
    void setGpx(Gpx* t){gpx = t;}
    void setWptMrk(WptMrk* wm) {wptmrk = wm;}

    Gpx* getGpx(){return gpx;}
public slots:
    void readGpx(QIODevice* dev, Gpx *t);
    void writeGPX(QIODevice* dev, Gpx* t);
private:
    void readGpxElement();
    void readMetadataElement();
    void readBoundsElement();
    void readTrkElement();
    void readNameElement();
    void readExtensionsElement();
    void readGpxxTrackExtensionElement();
    void readGpxxDisplayColorElement();
    void readTrkSegElement();
    void readTrkPtElement();
    void readEleElement();
    void readTimeElement();
    void readWptElement();
//    void readWptExtensionsElement();
//    void readWptMarkingElement();
//    void readWptFuelElement();
    void readWptDescElement();
    void readWptNamePosElement();
//    void readWptControlElement();
//    void readWptArrElement();
//    void readWptDepElement();
    void skipUnknownElement();
    void parsingDesc();
    void makeDesc(WayPoint* p);

    QDateTime time;
    double lat, lon, alt;
    QString name;
    QString desc;
    QString namepos;
    QString marking;
    double fuel;
    bool control;
    QDateTime arr;
    QDateTime dep;
    Gpx* gpx;
    QXmlStreamReader* reader;
    QXmlStreamWriter* writer;
    WptMrk* wptmrk;
};

#endif // GPXREADER_H
