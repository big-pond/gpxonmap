#ifndef GPXMODEL_H
#define GPXMODEL_H

#include <QAbstractTableModel>
#include <QList>

class Gpx;

class GpxModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    static const int COLCOUNT;
    static const int defaultColW[];
    explicit GpxModel(QObject *parent = nullptr);
    void setGpxList(QList<Gpx*> *gpxlst) {gpxlist = gpxlst;}

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void appendGpx(Gpx *gpx, const QModelIndex &parent = QModelIndex());
private:
    QList<Gpx*> *gpxlist;
};

#endif // GPXMODEL_H
