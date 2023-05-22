#ifndef WPTMODEL_H
#define WPTMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "geopoint.h"

class Gpx;
class WayPoint;

class WptModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    static const int COLCOUNT;
    static const int defaultColW[];
    explicit WptModel(QObject *parent = nullptr);
    void setGpx(Gpx* gpx);
//    Gpx* getGpx(){return gpx;}

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    WayPoint *wayPoint(int row);
    void sortByTime();
private:
    Gpx *gpx;
    GeoPoint::DispFormat cf;
};

#endif // WPTMODEL_H
