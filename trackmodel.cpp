#include "trackmodel.h"
#include "gpx.h"

const int TrackModel::COLCOUNT = 8;
const int TrackModel::defaultColW[] = {13, 14, 7, 18, 18, 9, 9, 7};

TrackModel::TrackModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    gpx = 0;
    cf = GeoPoint::Deg;
}

void TrackModel::setGpx(Gpx *gpx)
{
    beginResetModel();
    this->gpx = gpx;
    endResetModel();
}

QVariant TrackModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    QVariant v;
    if(orientation==Qt::Horizontal)
    {
        switch(section)
        {
        case Lat: v = tr("Latitude"); break;
        case Lon: v = tr("Longitude"); break;
        case Alt: v = tr("Alt, m"); break;
        case UTCTime: v = tr("Time UTC"); break;
        case 4: v = tr("Local time"); break;
        case 5: v = tr("t,s"); break;
        case 6: v = tr("V,km/h"); break;
        case 7: v = tr("L,m"); break;
        }
    }
    else if(orientation==Qt::Vertical)
        v = section + 1;
    return v;
}

int TrackModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    if (!gpx)
        return 0;
    return gpx->tptCount();
}

int TrackModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return COLCOUNT;
}

QVariant TrackModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(!gpx)
        return QVariant();


    TrackPoint p = gpx->getTrackPoint(index.row());
//    if(role==Qt::TextColorRole && gpx->isHasWaypoint(p))
//    {
//        return QColor("#aaffff");
//    }
    if(index.column()==Lat) //lat
    {
        if(role==Qt::TextAlignmentRole)  return int(Qt::AlignRight | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole) return  GeoPoint::latToStr(p.latitude, cf);
        else if(role==Qt::EditRole) return p.latitude;
    }
    else if(index.column()==Lon) //lon
    {
        if(role==Qt::TextAlignmentRole)  return int(Qt::AlignRight | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole) return  GeoPoint::lonToStr(p.longitude, cf);
        else if(role==Qt::EditRole) return p.longitude;
    }
    else if(index.column()==Alt) //высота
    {
        if(role==Qt::TextAlignmentRole)  return int(Qt::AlignRight | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole) return QString::number(p.altitude,'f',1);
        else if(role==Qt::EditRole) return p.altitude;
    }
    else if(index.column()==UTCTime) //время UTC
    {
        if(role==Qt::TextAlignmentRole)   return int(Qt::AlignLeft | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole)    return p.time.toString("dd.MM.yyyy hh:mm:ss");
        else if(role==Qt::EditRole) return p.time;
    }
    else if(index.column()==4) //время UTC + timezone + summertime
    {
        if(role==Qt::TextAlignmentRole) return int(Qt::AlignLeft | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole)  return p.time.addSecs(gpx->getLocalTimeOffset()*3600).toString("dd.MM.yyyy hh:mm:ss");
    }
    else if(index.column()>4/*&&index.row()>0*/)
    {
        double d, t;
        double v = gpx->getSpeed(index.row(), d, t);
        if(index.column()==5) //время секунд
        {
            if(role==Qt::TextAlignmentRole)  return int(Qt::AlignLeft | Qt::AlignVCenter);
            else if(role==Qt::DisplayRole)
            {
                QTime tm(0,0,0,0);
                tm = tm.addSecs(qRound(t));
                return tm.toString("hh:mm:ss");
            }
        }
        else if(index.column()==6)//скорость, км/ч
        {
            if(role==Qt::TextAlignmentRole)  return int(Qt::AlignRight | Qt::AlignVCenter);
            else if(role==Qt::DisplayRole)
                return QString("%1").arg(v,0,'f',2);
        }
        else if(index.column()==7) //пройденный путь,м
        {
            if(role==Qt::TextAlignmentRole)
                return int(Qt::AlignRight | Qt::AlignVCenter);
            else if(role==Qt::DisplayRole)
                return QString("%1").arg(d,0,'f',1);
        }
    }

    return QVariant();
}

bool TrackModel::insertRows(int row, int count, const QModelIndex &parent)
{
    bool result = false;
    if(row>=0 && row<=rowCount() && count>0)
    {
        beginInsertRows(parent, row, row + count - 1);
        while(count>0)
        {
            if(row<rowCount())
                gpx->trackPoints()->insert(row, TrackPoint());
            else
                gpx->trackPoints()->append(TrackPoint());
            count--;
        }
        result = count==0;
        gpx->setModified(true);
        endInsertRows();
    }
    return result;
}

bool TrackModel::removeRows(int row, int count, const QModelIndex &parent)
{
    bool result = false;
    if(row>=0 && row+count<=rowCount() && count>0)
    {
        beginRemoveRows(parent, row, row + count - 1);
        while(count>0)
        {
            gpx->trackPoints()->removeAt(row);
            count--;
        }
        result = count==0;
        gpx->setModified(true);
        endRemoveRows();
    }
    return result;
}

bool TrackModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool success = false;
    if(index.isValid()&&role==Qt::EditRole)
    {
        if(index.column()==Lat)
            gpx->trackPointAt(index.row())->latitude = value.toDouble(); // points[index.row()].latitude = value.toDouble();
        else if (index.column()==Lon)
            gpx->trackPointAt(index.row())->longitude = value.toDouble(); //points[index.row()].latitude = value.toDouble();
        else if (index.column()==Alt)
            gpx->trackPointAt(index.row())->altitude = value.toDouble(); //points[index.row()].altitude = value.toDouble();
        else if (index.column()==UTCTime)
            gpx->trackPointAt(index.row())->time = value.toDateTime(); //points[index.row()].time = value.toDateTime();
        emit dataChanged(index, index);
        success = true;
    }
    return success;
}

TrackPoint* TrackModel::trackPoint(int row)
{
    return gpx->trackPointAt(row);
}

void TrackModel::calcTimeOnInterval(int i1, int i2)
{
    gpx->calcTimeOnInterval(i1, i2);
    emit dataChanged(index(i1, UTCTime), index(i2, columnCount()-1));
}

void TrackModel::calcAltOnInterval(int i1, int i2)
{
    gpx->calcAltOnInterval(i1,i2);
    emit dataChanged(index(i1, Alt), index(i2, Alt/*columnCount()-1*/));
}
