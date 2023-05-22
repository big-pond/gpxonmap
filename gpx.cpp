#include <float.h>
#include "gpx.h"
#include "geopoint.h"
#include "util.h"

double Gpx::SPEED_STOP = 0.1;
double Gpx::SPEED_LOW = 30.0;

Gpx::Gpx(QObject *parent)
    : QObject(parent), filename(), color("#0000ff"), minlat(90), minlon(180), maxlat(-90), maxlon(-180),
        local_time_offset(0), trackpointvisible(false), tracklinevisible(true),
        drawspeed(false), drawstart(true), drawfinish(true),
        pointsize(4), linewidth(0), modified(false)
{

}

void Gpx::deleteAllPoints()
{
    trackpoints.clear();
    waypoints.clear();
}

void Gpx::checkMinMax(GeoPoint *point)
{
    if (point->latitude < minlat)
        minlat = point->latitude;
    if (point->latitude > maxlat)
        maxlat = point->latitude;
    if (point->longitude < minlon)
        minlon = point->longitude;
    if (point->longitude > maxlon)
        maxlon = point->longitude;
}

void Gpx::appendTpt(const TrackPoint& point)
{
    trackpoints.append(point);
    GeoPoint p = static_cast<GeoPoint>(point);
    checkMinMax(&p);
}

void Gpx::appendWpt(const WayPoint& point)
{
    waypoints.append(point);
    GeoPoint p = static_cast<GeoPoint>(point);
    checkMinMax(&p);
}

void Gpx::appendGpx(Gpx* gpx)
{
    trackpoints.append(gpx->trackpoints);
    std::sort(trackpoints.begin(), trackpoints.end());
    waypoints.append(gpx->waypoints);
    std::sort(waypoints.begin(), waypoints.end());
}

double Gpx::getTimeS()  // Время трека в секундах
{
    double secs = 0;
    if (trackpoints.count() > 0)
    {
        secs = trackpoints.first().time.msecsTo(trackpoints.last().time);
        if (secs > 0)
            secs /= 1000;
    }
    return secs;
}

QString Gpx::getTimeHMS()  // Время трека в формате 'h:m:s'
{
   qint64 msecs = 0;
   if (trackpoints.count()>0)
   {
       msecs = trackpoints.first().time.msecsTo(trackpoints.last().time);
   }
   return  msecToHMS(msecs);
}

bool Gpx::noBonds()
{
    return (minlat > maxlat) || (minlon > maxlon);
}

void Gpx::calcBounds()
{
    for(int i=0; i<trackpoints.count(); i++)
    {
        double lat = trackpoints[i].latitude;
        double lon = trackpoints[i].longitude;
        if (maxlat < lat)
            maxlat = lat;
        if (minlat > lat)
            minlat = lat;
        if (maxlon < lon)
            maxlon = lon;
        if (minlon > lon)
            minlon = lon;
    }
}

void Gpx::sortWaypoints()
{
    std::sort(waypoints.begin(), waypoints.end());
}

double Gpx::getDistance(int i1, int i2)
{
    double distance = 0.0;
    for(int i=i1; i<i2; i++)
        distance += trackpoints[i].sphDistanceTo(trackpoints[i + 1]);
    return distance;
}

double Gpx::getLength()
{
    return getDistance(0, trackpoints.count()-1);
}

double Gpx::getAltitudeMax(int& idx)
{
    double hmax = LOWEST;
    int count = trackpoints.size();
    for(int i=0; i<count; i++)
        if(hmax<trackpoints[i].altitude) { hmax = trackpoints[i].altitude; idx = i; }
    return hmax;
}

double Gpx::getAltitudeMin(int& idx)
{
    double hmin = HIGHEST;
    int count = trackpoints.size();
    for(int i=0; i<count; i++)
        if(hmin>trackpoints[i].altitude) { hmin = trackpoints[i].altitude; idx = i; }
    return hmin;
}

double Gpx::getSpeedMax(int& idx)
{
    int count = trackpoints.size();
    double ms, d, v = 0, vmax = 0;
    for(int i=0; i<count-1; i++)
    {
        ms = trackpoints[i].time.msecsTo(trackpoints[i+1].time);
        d = getDistance(i,i+1);
        if (ms>0) v = d/ms*3600;
        if(v>vmax) { vmax = v; idx = i;}
//        if(v>vmax) { vmax = v; idx = i+1;}
    }
    return vmax;
}

double Gpx::getSpeed(int i, double& d, double& t) //Возвращает скорость (км/ч), расстояние (м) и время (сек)
{
    double v = 0.0;
    t = 0.0;
    d = 0.0;
    if (i < trackpoints.count() - 1)
    {
        double ms = trackpoints[i].time.msecsTo(trackpoints[i+1].time);
        d = getDistance(i,i+1);
        if (ms>0)
        {
            v = d/ms*3600;
            t = ms/1000;
        }
    }
    return v;
}

double Gpx::getSpeed(int i)
{
    double v = 0.0;
    if (i < trackpoints.count() - 1)
    {
        double ms = trackpoints[i].time.msecsTo(trackpoints[i+1].time);
        double d = getDistance(i,i+1);
        if (ms>0)
            v = d/ms*3600;
    }
    return v;
}

double Gpx::getDistTimeForSpeedLessThan(int i1, int i2, double& time, const double& speed_low)
{
    double dist = 0.0;
    time = 0.0;
    for(int i=i1; i<i2-1; i++)
    {
        double d, t;
        double v = getSpeed(i, d, t);
        if (v<speed_low)
        {
            dist += d;
            time += t;
        }
    }
    return dist;
}

void Gpx::calcMoveStopTime(double& timemove, double& timestop, int idx1, int idx2, const double& speed_stop)
{
    timemove = timestop = 0;

    int idxend = idx2, count = trackpoints.count();
    if(idx2==0||idx2>count-1) idxend = count-1;
    for(int i = idx1; i<idxend; i++)
    {
        double d, t_sec;
        double v = getSpeed(i, d, t_sec);
        if((v>speed_stop))
            timemove += t_sec;
        else
            timestop += t_sec;
    }
}

double Gpx::findNextStop(int& startidx, int& endidx, double speed_stop)
{
    int count = trackpoints.size();
    double sums = 0, t, d, v = 0;
    for(; startidx<count-1; startidx++)
    {
        v = getSpeed(startidx, d, t);
        if (v<speed_stop)
        {
            sums += t;
            break;
        }
    }
    endidx=startidx;
    for(; endidx<count-1; endidx++)
    {
        v = getSpeed(startidx, d, t);
        if(v<speed_stop) sums += t;
        else break;
    }
    return sums;
}

int Gpx::findPointIndex(const QDateTime &t)
{
    int idx = -1;
    if (trackpoints.count() > 0)
    {
        int idx1 = 0;
        int idx2 = trackpoints.count() - 1;
        QDateTime t1 = trackpoints[idx1].time;
        QDateTime t2 = trackpoints[idx2].time;
        if (t1 <= t && t <= t2)
        {
            while (true)
            {
                int idx = (idx1 + idx2) / 2;
                if (t >= trackpoints[idx].time)
                    idx1 = idx;
                else
                    idx2 = idx;
                if ((idx2 - idx1) <= 1)
                    break;
            }
            QDateTime t1 = trackpoints[idx1].time;
            QDateTime t2 = trackpoints[idx2].time;
            if (t1.msecsTo(t) < t.msecsTo(t2))
                idx = idx1;
            else
                idx =idx2;
        }
    }
    return idx;
}

int Gpx::findNearestTrackPoint(const double &lat, const double &lon, double& dist)
{
    GeoPoint p(lat, lon);
    int idx = -1;
    dist = DBL_MAX;
    for(int i=0; i<trackpoints.count(); i++)
    {
         double s = p.sphDistanceTo(trackpoints[i]);
         if (dist>s)
         {
             dist = s;
             idx = i;
         }
    }
    return idx;
}

int Gpx::findNearestWayPoint(const double &lat, const double &lon, double &dist)
{
    GeoPoint p(lat, lon);
    int idx = -1;
    dist = DBL_MAX;
    for(int i=0; i<waypoints.count(); i++)
    {
         double s = p.sphDistanceTo(waypoints[i]);
         if (dist>s)
         {
             dist = s;
             idx = i;
         }
    }
    return idx;
}

int Gpx::nextWayPointIdx(int idx, int& tpt_index)
{
    tpt_index = -1;
    int wpt_index = -1;
    for(int i=idx+1; i< trackpoints.count(); i++)
    {
        for(int j=0; j<waypoints.count(); j++)
            if (trackpoints[i].time == waypoints[j].time)
            {
                tpt_index = i;
                wpt_index = j;
                break;
            }
        if (tpt_index > -1)
            break;
    }
    return wpt_index;
}

int Gpx::prevWayPointIdx(int idx, int& tpt_index)
{
    tpt_index = -1;
    int wpt_index = -1;
    for(int i=idx-1; i>-1; i--)
    {
        for(int j=0; j<waypoints.count(); j++)
            if (trackpoints[i].time == waypoints[j].time)
            {
                tpt_index = i;
                wpt_index = j;
                break;
            }
        if (tpt_index > -1)
            break;
    }
    return wpt_index;
}

bool Gpx::isHasWaypoint(int trk_point_idx)
{
    bool val = false;
    TrackPoint p = trackpoints[trk_point_idx];
    for(int i=0; i<waypoints.count(); i++)
    {
        if (p.time==waypoints[i].time)
        {
            val = true;
            break;
        }
    }
    return val;
}

bool Gpx::isHasWaypoint(const TrackPoint& p)
{
    bool val = false;
    for(int i=0; i<waypoints.count(); i++)
    {
        if (p.time==waypoints[i].time)
        {
            val = true;
            break;
        }
    }
    return val;
}

int Gpx::removeEquPoints()
{
    int n = 0;

    int i=0, js=0, count=0;
    bool calccount = false;
    while (i < trackpoints.count() - 5)
    {
        if (trackpoints[i].latitude == trackpoints[i+1].latitude
                && trackpoints[i].longitude == trackpoints[i+1].longitude
                && trackpoints[i].latitude == trackpoints[i+2].latitude
                && trackpoints[i].longitude == trackpoints[i+2].longitude
                && trackpoints[i].latitude == trackpoints[i+3].latitude
                && trackpoints[i].longitude == trackpoints[i+3].longitude
                && trackpoints[i].latitude == trackpoints[i+4].latitude
                && trackpoints[i].longitude == trackpoints[i+4].longitude)
        {
            if (!calccount)
            {
                js = i + 2;
                calccount = true;
                count = 0;
                i++;
            }
            else
            {
                count++;
                i++;
            }
        }
        else
        {
            calccount = false;
            i++;
            /* для Qt 6.2.0
            if (count > 0)
            {
                trackpoints.remove(js, count);
                i = js;
                n += count;
                count = 0;
            }
             */
            if (count > 0)
            {
                while (count>0) {
                    trackpoints.removeAt(js);
                    n++;
                    count--;
                }
                i = js;
            }
        }
    }
    if (n > 0)
        modified = true;
    return n;
}

int Gpx::removeAzPoints(const double& da, const double& ds)
{
    // Удаляет точку p если азимут на нее меньше da и линия таектории после удаления p пройдет
    // от нее на расстоянии меньше ds
    int n = 0;
    int i = 0;
    while (i < trackpoints.count() - 2)
    {
        if (trackpoints[i+1].latitude == trackpoints[i+2].latitude
                && trackpoints[i+1].longitude == trackpoints[i+2].longitude)
            i += 3;
        else
        {
            double az1 = trackpoints[i].sphAzimuthTo(trackpoints[i + 1]);
            double az2 = trackpoints[i + 1].sphAzimuthTo(trackpoints[i + 2]);
            double az3 = trackpoints[i].sphAzimuthTo(trackpoints[i + 2]);
            double d = trackpoints[i].sphDistanceTo(trackpoints[i + 1]);
            GeoPoint p = trackpoints[i].atSphDistanceAndAzimuth(d, az3);
            double d_s = trackpoints[i + 1].sphDistanceTo(p);
            double delta = abs(az1 - az2);
            if (delta < da && d_s < ds)
            {
                trackpoints.removeAt(i + 1);
                n++;
            }
            else
                i += 1;
        }
    }
    if (n > 0)
        modified = true;
    return n;
}

void Gpx::calcTimeOnInterval(int i1, int i2)
{
    QDateTime t1 = trackpoints[i1].time;
    QDateTime t2 = trackpoints[i2].time;
    double dmsecs = t1.msecsTo(t2);
    double s = getDistance(i1, i2);
    for(int i=i1+1; i<i2; i++)
    {
        double s1 = getDistance(i1, i);
        qint64 msecs = qRound(s1/s*dmsecs);
        trackpoints[i].time = t1.addMSecs(msecs);
    }
}

void Gpx::calcAltOnInterval(int i1, int i2)
{
    double h1 = trackpoints[i1].altitude;
    double h2 = trackpoints[i2].altitude;
    double dh = h2-h1;
    double s = getDistance(i1, i2);
    for(int i=i1+1; i<i2; i++)
    {
        double s1 = getDistance(i1, i);
        trackpoints[i].altitude = h1 +  s1/s*dh;
    }
}

int Gpx::nextControlWayPointIndex(int waypoint_index)
{
    int next_way_point_index = -1;
    for (int i = waypoint_index + 1; i< waypoints.count(); i++)
        if (waypoints[i].control())
        {
            next_way_point_index = i;
            break;
        }
    return next_way_point_index;
}

void Gpx::controlIntervalData(int i1, int i2,     // i1, i2 - indexes of control waypoints
                              double& dist, double& move_t, double& speed,
                              double& stop_it, double& stop_id, double& trafjam_t, double& trafjam_d)
{
    QList<WayPoint*> wpt_list;
    for (int i=i1; i<=i2; i++)  // append waypoints with indexes i1 and i2 and all (not control) located between them
        wpt_list << &(waypoints[i]);
    dist = stop_id = move_t = speed = stop_it = trafjam_t = trafjam_d = 0.0;
    for (int i=0; i<wpt_list.count() - 1; i++)
    {
        int ti1 = findPointIndex(wpt_list[i]->dep());
        int ti2 = findPointIndex(wpt_list[i + 1]->arr());
        dist += getDistance(ti1, ti2);
        move_t += wpt_list[i]->dep().msecsTo(wpt_list[i+1]->arr())*0.001;
        if (i+1 < wpt_list.count()-1)  // stop time & distance at a control point is considered outside of this function
        {
            stop_it += wpt_list[i+1]->arr().msecsTo(wpt_list[i+1]->dep())*0.001;
            int pi1 = findPointIndex(wpt_list[i + 1]->arr());
            int pi2 = findPointIndex(wpt_list[i + 1]->dep());
            stop_id += getDistance(pi1, pi2);
        }
        trafjam_d = getDistTimeForSpeedLessThan(ti1, ti2, trafjam_t);
    }
    dist -= stop_id;
    if (move_t != 0)
            speed = dist / move_t * 3.6;  // km/h
}

void Gpx::calcCommonCharact(double& timemove, double& distmove,  double& timestop, double& distmovestop,
                            int idx1, int idx2, const double& speed_stop)
{
    timemove = 0;
    distmove = 0;
    timestop = 0;
    distmovestop = 0;
    int idx_end = idx2;
    int count = trackpoints.count();
    if (idx2 == 0 || idx2 > count - 1)
        idx_end = count - 1;
    for (int i=idx1; i<idx_end; i++)
    {
        double d, sec;
        double v = getSpeed(i, d, sec);
        double dist = getDistance(i, i + 1);
        if (v > speed_stop)
        {
            timemove += sec;
            distmove += dist;
        }
        else
        {
            timestop += sec;
            distmovestop += dist;
        }
    }
}

void Gpx::averagingTrackCoord(int i1, int i2)
{
    if (i2 - i1 > 1)
    {
        double lt = 0.0, ln = 0.0, el = 0.0;
        for (int i=i1; i<=i2; i++)
        {
            lt += trackpoints[i].latitude;
            ln += trackpoints[i].longitude;
            el += trackpoints[i].altitude;
        }
        int n = (i2 - i1) + 1;
        lt /= n;
        ln /= n;
        el /= n;
        for (int i=i1; i<=i2; i++)
        {
            trackpoints[i].latitude = lt;
            trackpoints[i].longitude = ln;
            trackpoints[i].altitude = el;
        }
    }
}

void Gpx::equateTrackCoordToPrec(int i1, int i2)
{
    if (i2 - i1 > 1 && i1 > 0)
    {
        TrackPoint p = trackpoints[i1 - 1];
        for (int i=i1; i<=i2; i++)
        {
            trackpoints[i].latitude = p.latitude;
            trackpoints[i].longitude = p.longitude;
            trackpoints[i].altitude = p.altitude;
        }
    }
}

void Gpx::equateTrackCoordToAfter(int i1, int i2)
{
    if (i2 - i1 > 1 && i2 < trackpoints.count() - 1)
    {
        TrackPoint p = trackpoints[i2 + 1];
        for (int i=i1; i<=i2; i++)
        {
            trackpoints[i].latitude = p.latitude;
            trackpoints[i].longitude = p.longitude;
            trackpoints[i].altitude = p.altitude;
        }
    }
}

Gpx* Gpx::getPartCopy(int i1, int i2)
{
    Gpx* gpx = new Gpx();
    QString pathname = filename.section('.',0,-2);
    QString suffix = filename.section('.',-1);
    gpx->setFileName(QString("%1_part_%2_%3.%4").arg(pathname).arg(i1).arg(i2).arg(suffix));
    gpx->setName(QString("%1_part_%2_%3").arg(name).arg(i1).arg(i2));
    gpx->setColor ("#ff0000");
    gpx->setTrackPointVisible(false);
    gpx->setTrackLineVisible(true);
    gpx->setDrawSpeed(false);
    gpx->setDrawStart(true);
    gpx->setDrawFinish(true);
    gpx->setPointSize(4);
    gpx->setLineWidth(0);
    gpx->setModified (true);
    for(int i=i1; i<=i2; i++)
    {
        gpx->appendTpt(TrackPoint(trackpoints[i]));
    }
    QDateTime t1 = trackpoints[i1].time;
    QDateTime t2 = trackpoints[i2].time;
    for(int i=0; i<waypoints.count(); i++)
        if(waypoints[i].time>=t1 && waypoints[i].time<=t2)
        {
            gpx->appendWpt(WayPoint(waypoints[i]));
        }
    gpx->calcBounds();
    return gpx;
}

void Gpx::calcArrDepTime(const QDateTime& time, QDateTime& arr_t, QDateTime& dep_t)
{
    int idx = findPointIndex(time);
    arr_t = dep_t = time;
    if (idx >= 0)
    {
        arr_t = calcArrTime(idx);
        dep_t = calcDepTime(idx);
    }
}

QDateTime Gpx::calcArrTime(int i, const double& speed_stop)
{
    double v = 0.0;
    do
    {
        i--;
        if (i < 1)
            break;
        v = getSpeed(i);
    }
    while(v < speed_stop);
    return trackpoints[i + 1].time;
}

QDateTime Gpx::calcDepTime(int i, const double& speed_stop)
{
        double v = 0.0;
        do
        {
            i++;
            if (i > trackpoints.count() - 1)
                break;
            v = getSpeed(i);
        }
        while (v < speed_stop);
        return trackpoints[i - 1].time;
}
