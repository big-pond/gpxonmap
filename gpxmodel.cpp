#include "gpxmodel.h"
#include "gpx.h"

const int GpxModel::COLCOUNT = 5;
const int GpxModel::defaultColW[] = {5, 50, 9, 9, 9};

GpxModel::GpxModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant GpxModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) return QVariant();
    QVariant v;
    if(orientation==Qt::Horizontal)
    {
        switch(section)
        {
        case 0: v = tr("C"); break;
        case 1: v = tr("File"); break;
        case 2: v = tr("Point\ncount"); break;
        case 3: v = tr("Time"); break;
        case 4: v = tr("L, km"); break;
        }
    }
    else if(orientation==Qt::Vertical)
        v = section + 1;
   return v;
}

int GpxModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return gpxlist->count();
}

int GpxModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return COLCOUNT;
}

QVariant GpxModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(index.column()==0) //цвет
     {
         if(role==Qt::TextAlignmentRole)
             return int(Qt::AlignRight | Qt::AlignVCenter);
          else if (role==Qt::DecorationRole)
             return QColor(gpxlist->at(index.row())->getColor());
     }
     else if(index.column()==1) //имя файла (с путем)
     {
         if(role==Qt::TextAlignmentRole)
             return int(Qt::AlignLeft | Qt::AlignVCenter);
         else if(role==Qt::DisplayRole)
             return gpxlist->at(index.row())->getFileName();
     }
    else if(index.column()==2) //точек
    {
        if(role==Qt::TextAlignmentRole)
            return int(Qt::AlignRight | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole)
            return gpxlist->at(index.row())->tptCount();
    }
    else if(index.column()==3) //время
    {
        if(role==Qt::TextAlignmentRole)
            return int(Qt::AlignRight | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole)
            return gpxlist->at(index.row())->getTimeHMS();
    }
     else if(index.column()==4) //длина
     {
         if(role==Qt::TextAlignmentRole)
             return int(Qt::AlignRight | Qt::AlignVCenter);
         else if(role==Qt::DisplayRole)
             return QString::number(gpxlist->at(index.row())->getLength()*0.001,'f',3);
     }
    return QVariant();
}

void GpxModel::appendGpx(Gpx* gpx, const QModelIndex& parent)
{
        int rc = gpxlist->count();
        beginInsertRows(parent, rc, rc);
        gpxlist->append(gpx);
        endInsertRows();
}

bool GpxModel::removeRows(int row, int count, const QModelIndex& parent)
{
        int size = gpxlist->count();
        if (size > 0)
            if ((row >= 0) && ((row + count - 1) < size))
            {
                beginRemoveRows(parent, row, row + count - 1);
//                gpxlist->remove(row, count);  Qt 6.2.0 вместо цикла while ниже
                while(count>0)
                {
                    gpxlist->removeAt(row);
                    count--;
                }
                endRemoveRows();
            }
        return gpxlist->count() < size;
}

