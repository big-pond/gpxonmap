#ifndef DATAWDG_H
#define DATAWDG_H

#include <QWidget>
#include <QList>

namespace Ui {
class DataWdg;
}

class TrackPoint;
class WayPoint;
class Gpx;
class GpxModel;
class TrackModel;
class WptModel;
class WptMrk;
class QSettings;
class QTableView;

class DataWdg : public QWidget
{
    Q_OBJECT

public:
    static int go_step;
    static bool remove_in_model;
    explicit DataWdg(QList<Gpx*> *gpxlst, QWidget *parent = nullptr);
    ~DataWdg();
    void setWptMrk(WptMrk* val){wptmrk = val;}
    void appendGpx(Gpx* gpx);
    QList<Gpx*>* getGpxList(){return gpxlist;}
    Gpx* getMeasure(){return measure;}
    TrackModel* getTrackModel(){return trackmodel;}
    QTableView* getTrackTableView();
    Gpx *getCurrentGpx();
    QList<Gpx*> closeGpx();
    int getCurrentTrackPointRow();
    int findNearestPointIndex(const QDateTime &t);
    TrackPoint *getCurrentTrackPoint();
    WayPoint *getCurrentWayPoint();
    void setCurrentTrackPoint(int i);
    QString getGpxFileNames();
    void mergeGpx(Gpx *gpx);
//    void appendTrackPoint(TrackPoint *p);
    bool appendTrackPoint(const double& lat,const double& lon);
    bool insertTrackPoint(const double& lat, const double& lon);
    void changePointCoordinates(const double &lat, const double &lon);
    void writeSettings(QSettings &settings);
    void readSettings(QSettings &settings);
public slots:
    void gpxCurrentRowChanged(const QModelIndex &current, const QModelIndex &);
    void trackProperty();
    void defaultGpxColumnWidth();
    void defaultTrkColumnWidth();
    void defaultWptColumnWidth();
    void moveGpxUp();
    void moveGpxDown();
    void firstTrackPoint();
    void prevTrackPoint();
    void nextTrackPoint();
    void lastTrackPoint();
    void goNPoints();
    void findSpeedMax();
    void findAltMax();
    void findAltMin();
    void findNextStop();
    void catGpx();
    void removeEqPoints();
    void removeAzPoints();
    void deleteTrackPoints();
    void deletePointsAbove();
    void deletePointsBelow();
    void averagingTrackCoord();
    void equateTrackCoordToPrec();
    void equateTrackCoordToAfter();
    void calcTimeOnInterval();
    void prevWayPoint();
    void nextWayPoint();
    void toPrevWayPoint();
    void toNextWayPoint();
    void editTrackPoint();
    void appendWayPoint();
    void editWayPoint();
    void deleteWayPoints();
    void sortWayPoints();
    void alignWptTpt();
    void setArrivalTime();
    void setDepartureTime();
    void hideTrkColumns();
    void hideWptColumns();
private:
    Ui::DataWdg *ui;
    GpxModel* gpxmodel;
    TrackModel* trackmodel;
    WptModel* wptmodel;
    WptMrk* wptmrk;

    QList<Gpx*> *gpxlist;
    Gpx* measure;
    int removeEquPoints();
    int removeAzPoints(const double &da, const double &ds);
};

#endif // DATAWDG_H
