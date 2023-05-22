#include "wptmodel.h"
#include "gpx.h"
#include "util.h"

const int WptModel::COLCOUNT = 10;
const int WptModel::defaultColW[] = {5, 13, 14, 7, 18, 18, 22, 10, 6, 7};

WptModel::WptModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    gpx = 0;
    cf = GeoPoint::Deg;
}

void WptModel::setGpx(Gpx *gpx)
{
    beginResetModel();
    this->gpx = gpx;
    endResetModel();
}

QVariant WptModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) return QVariant();
    QVariant v;
    if(orientation==Qt::Horizontal)
    {
        switch(section)
        {
        case 0: v = tr("Sign"); break;
        case 1: v = tr("Latitude"); break;
        case 2: v = tr("Longitude"); break;
        case 3: v = tr("Alt, м"); break;
        case 4: v = tr("Time UTC"); break;
        case 5: v = tr("Local times"); break;
        case 6: v = tr("Name"); break;
        case 7: v = tr("Stop\ntime"); break;
        case 8: v = tr("Fuel"); break;
        case 9: v = tr("L, km"); break;
        }
    }
    else if(orientation==Qt::Vertical)
        v = section + 1;
    return v;
}

int WptModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    if (!gpx)
        return 0;
    return gpx->wptCount();
}

int WptModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return COLCOUNT;
}

QVariant WptModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(!gpx)
        return QVariant();
    WayPoint p = gpx->getWayPoint(index.row());
    if(index.column()==0) //значек
    {
        if(role==Qt::TextAlignmentRole)
            return int(Qt::AlignRight | Qt::AlignVCenter);
        else if(role==Qt::DecorationRole)
            return *p.image();
    }
    if(index.column()==1) //lat
    {
        if(role==Qt::TextAlignmentRole)  return int(Qt::AlignRight | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole) return GeoPoint::latToStr(p.latitude, cf);
    }
    else if(index.column()==2) //lon
    {
        if(role==Qt::TextAlignmentRole)  return int(Qt::AlignRight | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole) return GeoPoint::lonToStr(p.longitude, cf);
    }
    else if(index.column()==3) //высота
    {
        if(role==Qt::TextAlignmentRole)  return int(Qt::AlignRight | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole) return QString::number(p.altitude,'f',1);
    }
    else if(index.column()==4) //время UTC
    {
        if(role==Qt::TextAlignmentRole)   return int(Qt::AlignLeft | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole)    return p.time.toString("dd.MM.yyyy hh:mm:ss");
    }
    else if(index.column()==5) //время UTC + timezone + summertime
    {
        if(role==Qt::TextAlignmentRole) return int(Qt::AlignLeft | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole)  return p.time.addSecs(gpx->getLocalTimeOffset()*3600).toString("dd.MM.yyyy hh:mm:ss");
    }
    else if(index.column()==6) //name
    {
        if(role==Qt::TextAlignmentRole) return int(Qt::AlignLeft | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole)  return p.name();
    }
    else if(index.column()==7) //Время стоянки
    {
        if(role==Qt::TextAlignmentRole) return int(Qt::AlignRight | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole)
        {
            QString s;
            qint64 ms = p.arr().msecsTo(p.dep());
            if (ms > 0) s = msecToHMS(ms);
            return s;
        }
    }
    else if(index.column()==8) //Топливо
    {
        if(role==Qt::TextAlignmentRole) return int(Qt::AlignRight | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole)
        {
            if(p.isFuel()) return QString::number(p.fuel(),'f',1);
            else return QVariant();
        }
    }
    else if(index.column()==9) //Расстояние от пред. точки
    {
        if(role==Qt::TextAlignmentRole) return int(Qt::AlignRight | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole)
        {
            double d = 0.0;
            if (index.row() > 0)
            {
                QDateTime t1 = gpx->wayPointAt(index.row() - 1)->dep();
                QDateTime t2 = p.arr();
                int i1 = gpx->findPointIndex(t1);
                int i2 = gpx->findPointIndex(t2);
                d = gpx->getDistance(i1, i2);
                if (d > 0)
                    return QString("%1").arg(d*0.001,0,'f',1);
                else
                    return QVariant();
            }
            else
                return QVariant();
        }
    }
    return QVariant();
}

bool WptModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool success = false;
    if (index.isValid() && role == Qt::EditRole)
    {
        if (index.column() == 1)  // 0 - sign
            gpx->wayPointAt(index.row())->latitude = value.toDouble();
        if (index.column() == 2)
            gpx->wayPointAt(index.row())->longitude = value.toDouble();
        if (index.column() == 3)
            gpx->wayPointAt(index.row())->altitude = value.toDouble();
        if (index.column() == 4)
            gpx->wayPointAt(index.row())->time = value.toDateTime();
        if (index.column() == 6)
            gpx->wayPointAt(index.row())->setName(value.toString());
        success = true;
        emit dataChanged(index, index);
    }
    return success;
}

bool WptModel::insertRows(int row, int count, const QModelIndex &parent)
{
    bool result = false;
    if(row>=0 && row<=rowCount() && count>0)
    {
        beginInsertRows(parent, row, row + count - 1);
        while(count>0)
        {
            if(row<rowCount())
                gpx->wayPoints()->insert(row, WayPoint());
            else
                gpx->wayPoints()->append(WayPoint());
            count--;
        }
        result = count==0;
        gpx->setModified(true);
        endInsertRows();
    }
    return result;
}

bool WptModel::removeRows(int row, int count, const QModelIndex &parent)
{
    bool result = false;
    if(row>=0 && row+count<=rowCount() && count>0)
    {
        beginRemoveRows(parent, row, row + count - 1);
        while(count>0)
        {
            gpx->wayPoints()->removeAt(row);
            count--;
        }
        result = count==0;
        gpx->setModified(true);
        endRemoveRows();
    }
    return result;
}

WayPoint* WptModel::wayPoint(int row)
{
    return gpx->wayPointAt(row);
}

void WptModel::sortByTime()
{
    gpx->sortWaypoints();
    emit dataChanged(this->index(0, 0), this->index(0, rowCount()-1));
}
