#ifndef TRACKMODEL_H
#define TRACKMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "geopoint.h"

class Gpx;
class TrackPoint;

class TrackModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    static const int COLCOUNT;
    static const int defaultColW[];
    enum {Lat, Lon, Alt, UTCTime};
    explicit TrackModel(QObject *parent = nullptr);
    void setGpx(Gpx* gpx);
//    Gpx* getGpx(){return gpx;}

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    void setPointDispFormat(/*GPoint::DispFormat*/int val);
    int getPointDispFormat(){return cf;}
    TrackPoint *trackPoint(int row);
    void calcTimeOnInterval(int i1, int i2);
    void calcAltOnInterval(int i1, int i2);
private:
    Gpx *gpx;

    GeoPoint::DispFormat cf;
//    QList<TrackPoint> *trackpoints;

};

#endif // TRACKMODEL_H
