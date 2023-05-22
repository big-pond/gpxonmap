#ifndef GPX_H
#define GPX_H

#include <QObject>

#include "trackpoint.h"
#include "waypoint.h"

class Gpx : public QObject
{
    Q_OBJECT
public:
    static double SPEED_STOP;
    static double SPEED_LOW;
    explicit Gpx(QObject *parent = nullptr);
    QString getFileName() {return filename;}
    void setFileName(const QString& val) {filename = val;}
    QString getName() {return name;}
    void setName(const QString& val) {name = val;}
    QString getColor() {return color;}
    void setColor(const QString& val) {color = val;}
    double getMinLat(){return minlat;}
    void setMinLat(double val){minlat = val;}
    double getMinLon(){return minlon;}
    void setMinLon(double val){minlon = val;}
    double getMaxLat(){return maxlat;}
    void setMaxLat(double val){maxlat = val;}
    double getMaxLon(){return maxlon;}
    void setMaxLon(double val){maxlon = val;}
    int getLocalTimeOffset() {return local_time_offset;}
    void setLocalTimeOffset(int val){local_time_offset = val;}
    bool getTrackPointVisible(){return trackpointvisible;}
    void setTrackPointVisible(bool val){trackpointvisible = val;}
    bool getTrackLineVisible(){return tracklinevisible;}
    void setTrackLineVisible(bool val){tracklinevisible = val;}
    bool getDrawSpeed(){return drawspeed;}
    void setDrawSpeed(bool val){drawspeed = val;}
    bool getDrawStart(){return drawstart;}
    void setDrawStart(bool val){drawstart = val;}
    bool getDrawFinish(){return drawfinish;}
    void setDrawFinish(bool val){drawfinish = val;}
    int getPointSize(){return pointsize;}
    void setPointSize(int val){pointsize = val;}
    int getLineWidth(){return linewidth;}
    void setLineWidth(int val){linewidth = val;}
    bool isModified(){return modified;}
    void setModified(bool val){modified = val;}
    void deleteAllPoints(); // trakc points and waypoints

    void checkMinMax(GeoPoint *point);
    int tptCount(){return trackpoints.count();}
    TrackPoint getTrackPoint(int i){return trackpoints[i];}
    TrackPoint* trackPointAt(int i){return &trackpoints[i];}
    int wptCount(){return waypoints.count();}
    WayPoint getWayPoint(int i){return waypoints[i];}
    WayPoint* wayPointAt(int i){return &waypoints[i];}
    QList<TrackPoint>* trackPoints(){return &trackpoints;}
    QList<WayPoint>* wayPoints(){return &waypoints;}
    Gpx *getPartCopy(int i1, int i2);
    void appendTpt(const TrackPoint &point);
    void appendWpt(const WayPoint &point);
    void appendGpx(Gpx *gpx);
    double getTimeS();
    QString getTimeHMS();
    bool noBonds();
    void calcBounds();
    void sortWaypoints();
    double getDistance(int i1, int i2);
    double getLength();
    double getAltitudeMax(int &idx);
    double getAltitudeMin(int &idx);
    double getSpeedMax(int &idx);
    double getSpeed(int i, double &d, double &t);// Возвращает скорость (км/ч), расстояние (м) и время (сек)
    double getSpeed(int i); // Возвращает скорость (км/ч)
    double getDistTimeForSpeedLessThan(int i1, int i2, double &time, const double &speed_low=SPEED_LOW);
    void calcMoveStopTime(double &timemove, double &timestop, int idx1=0, int idx2=0, const double &speed_stop=SPEED_STOP);
    double findNextStop(int &startidx, int &endidx, double speed_stop=SPEED_STOP);
    int findPointIndex(const QDateTime& t);
    int findNearestTrackPoint(const double& lat, const double& lon, double& dist);
    int findNearestWayPoint(const double& lat, const double& lon, double& dist);
    int nextWayPointIdx(int idx, int &tpt_index);
    int prevWayPointIdx(int idx, int &tpt_index);
    bool isHasWaypoint(int trk_point_idx);
    bool isHasWaypoint(const TrackPoint &p);
    void calcTimeOnInterval(int i1, int i2);
    void calcAltOnInterval(int i1, int i2);
    void calcCommonCharact(double &timemove, double &distmove, double &timestop, double &distmovestop,
                           int idx1, int idx2, const double &speed_stop=SPEED_STOP);
    int nextControlWayPointIndex(int waypoint_index);
    void controlIntervalData(int i1, int i2, double &dist, double &move_t, double &speed,
                             double &stop_it, double &stop_id, double &trafjam_t, double &trafjam_d);
    int removeEquPoints();
    int removeAzPoints(const double &da, const double &ds);
    void averagingTrackCoord(int i1, int i2);
    void equateTrackCoordToPrec(int i1, int i2);
    void equateTrackCoordToAfter(int i1, int i2);
    QDateTime calcArrTime(int i, const double &speed_stop=SPEED_STOP);
    QDateTime calcDepTime(int i, const double &speed_stop=SPEED_STOP);
    void calcArrDepTime(const QDateTime &time, QDateTime &arr_t, QDateTime &dep_t);
signals:
protected:
    QString filename;
    QString name;
    QString color;
    double minlat;
    double minlon;
    double maxlat;
    double maxlon;
    int local_time_offset = 0; //часы
    bool trackpointvisible = false;
    bool tracklinevisible = true;
    bool drawspeed = false;
    bool drawstart = true;
    bool drawfinish = true;
    int pointsize = 4;
    int linewidth = 0;
    bool modified = false;

    QList<TrackPoint> trackpoints;
    QList<WayPoint> waypoints;

};

#endif // GPX_H
