#include <QFile>
#include <QTextStream>
#include <QIcon>
#include "wptmrk.h"
#include "waypoint.h"

#include <QDebug>

//Маркировка marking
const QString WptMrk::ARR = "ARR";
const QString WptMrk::BED = "BED";
const QString WptMrk::CAM = "CAM";
const QString WptMrk::CONTROL = "CONTROL";
const QString WptMrk::DEP = "DEP";
const QString WptMrk::FINISH = "FINISH";
const QString WptMrk::FOOD = "FOOD";
const QString WptMrk::FUEL = "FUEL";
const QString WptMrk::GASSTATION = "GASSTATION";
const QString WptMrk::HMAX = "HMAX";
const QString WptMrk::HMIN = "HMIN";
const QString WptMrk::LABEL = "LABEL";
const QString WptMrk::POINT = "POINT";
const QString WptMrk::POLICE = "POLICE";
const QString WptMrk::SPEEDMAX = "SPEEDMAX";
const QString WptMrk::START = "START";
const QString WptMrk::STOP = "STOP";
const QString WptMrk::TANKFUEL = "TANKFUEL";
const QString WptMrk::TRAFLIGHT = "TRAFLIGHT";

//Положение подписей
const QString WptMrk::BOTTOMRIGHT = "BOTTOMRIGHT";
const QString WptMrk::BOTTOM = "BOTTOM";
const QString WptMrk::BOTTOMLEFT = "BOTTOMLEFT";
const QString WptMrk::LEFT = "LEFT";
const QString WptMrk::TOPLEFT = "TOPLEFT";
const QString WptMrk::TOP = "TOP";
const QString WptMrk::TOPRIGHT = "TOPRIGHT";
const QString WptMrk::RIGHT = "RIGHT";


WptMrk::WptMrk(QObject *parent) : QAbstractTableModel(parent), colcount(5)
{
    readData("wptmrk.dat");
    loadImages();
}

int	WptMrk::rowCount(const QModelIndex&) const {return det.count();}

int	WptMrk::columnCount(const QModelIndex&) const {return colcount;}

QVariant WptMrk::data(const QModelIndex& index, int role) const
{
    if(!index.isValid()) return QVariant();
    if(index.column()==0) //sign
    {
        if(role==Qt::TextAlignmentRole) return int(Qt::AlignRight | Qt::AlignVCenter);
        else if(role==Qt::DecorationRole) return det.at(index.row()).image;
    }
    else if(index.column()==1) //marking
    {
        if(role==Qt::TextAlignmentRole)
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole/*||role==Qt::EditRole*/)
            return det.at(index.row()).marking;
    }
    else if(index.column()==2) //name
    {
        if(role==Qt::TextAlignmentRole)
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole/*||role==Qt::EditRole*/)
            return det.at(index.row()).name;
    }
    else if(index.column()==3) //dx
    {
        if(role==Qt::TextAlignmentRole)
            return int(Qt::AlignRight | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole/*||role==Qt::EditRole*/)
            return det.at(index.row()).dx;
    }
    else if(index.column()==4) //dy
    {
        if(role==Qt::TextAlignmentRole)
            return int(Qt::AlignRight | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole)
            return det.at(index.row()).dy;
    }
    else if(index.column()==5) //path
    {
        if(role==Qt::TextAlignmentRole)
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        else if(role==Qt::DisplayRole/*||role==Qt::EditRole*/)
            return det.at(index.row()).imagepath;
    }
    return QVariant();
}

QVariant WptMrk::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) return QVariant();
    QVariant v;
    if(orientation==Qt::Horizontal)
    {
        switch(section)
        {
        case 0: v = tr("Sign"); break;
        case 1: v = tr("Marking"); break;
        case 2: v = tr("Name"); break;
        case 3: v = "dx"; break;
        case 4: v = "dy"; break;
        case 5: v = tr("Image path"); break;
        }
    }
    else if(orientation==Qt::Vertical)
        v = section + 1;
    return v;
}

void WptMrk::readData(const QString& fname)
{
    QFile file(fname);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
//        stream.setCodec("UTF-8");
        QString str;
        QChar sep(';');
        while(!stream.atEnd())
        {
            str = stream.readLine();
            str = str.trimmed();
            SymRec sr;
            sr.marking = str.section(sep,0,0);
            sr.name = str.section(sep,1,1);
            sr.dx = str.section(sep,2,2).toInt();
            sr.dy = str.section(sep,3,3).toInt();
            sr.imagepath = str.section(sep,4,4);
            det << sr;
        }
    }
    else
        setDefault();
}

void WptMrk::loadImages()
{
    for(int i=0; i<det.count(); i++)
    {
        det[i].image.load(det[i].imagepath);
    }
}

void WptMrk::setDefault()
{
//    det << SymRec(SYM_FLAG_BLUE,   -4, -14, ":/resources/sflagb.png");
//    det << SymRec(SYM_FLAG_GREEN,  -4, -14, ":/resources/sflagg.png");
//    det << SymRec(SYM_FLAG_RED,    -4, -14, ":/resources/sflagr.png");
    det << SymRec(POINT, tr("Point of trip"), -4, -14, ":/resources/sflagy.png");
    det << SymRec(BED,  tr("Sleep"),  -7,  -15, ":/resources/sbed.png");
    det << SymRec(GASSTATION,  tr("Fueling station"),  -15,  -14, ":/resources/sfuel.png");
    det << SymRec(FUEL,  tr("Refueling the car"),  -15,  -14, ":/resources/sfuelcar.png");
    det << SymRec(STOP,    tr("Stop"),-1,  -15, ":/resources/sstop.png");
    det << SymRec(FOOD, tr("Food intake"),    -7,  -15, ":/resources/sfood.png");
    det << SymRec(POLICE, tr("Police"),   -7,  15, ":/resources/spolice.png");
    det << SymRec(TRAFLIGHT, tr("Traffic light"),   -7,  -15, ":/resources/strafligt.png");
    det << SymRec(START, tr("Start of trip"),    -1,  -15, ":/resources/start16.png");
    det << SymRec(FINISH, tr("Finish of trip"),   -1,  -15, ":/resources/finish16.png");
    det << SymRec(CAM, tr("Camera"),   -7,  -15, ":/resources/scam.png");
    det << SymRec(HMAX, tr("Maximum altitude"),   -7,  -15, ":/resources/shmax.png");
    det << SymRec(HMIN, tr("Minimum altitude"),   -7,  -15, ":/resources/shmin.png");
    det << SymRec(SPEEDMAX, tr("Maximum speed"),   -7,  -15, ":/resources/svmax.png");

}

QImage* WptMrk::getImage(const QString mark)
{
    QImage* im = &det[0].image;
    for (int i=1; i<det.count(); i++)
        if (mark==det[i].marking)
        {
            im = &det[i].image;
            break;
        }
    return im;
}

QString WptMrk::getImagePath(const QString mark)
{
    QString imp;
    for (int i=1; i<det.count(); i++)
        if (mark==det[i].marking)
        {
            imp = det[i].imagepath;
            break;
        }
    return imp;
}

int WptMrk::markingIndex(const QString &mark)
{
    int idx = -1;
    for (int i=0; i<det.count(); i++)
        if (mark==det[i].marking)
        {
            idx = i;
            break;
        }
    return idx;
}

bool WptMrk::isMarking(const QString &mark)
{
    bool res = false;
    for (int i=0; i<det.count(); i++)
    {
        res = (mark==det[i].marking);
        if (res) break;
    }
    return res;
}

void WptMrk::setImageToWayPoint(WayPoint* wpt)
{
    QImage* im = &det[0].image;
    int dx = det[0].dx;
    int dy = det[0].dy;
    QString mark = wpt->marking();
    for (int i=1; i<det.count(); i++)
        if (mark==det[i].marking)
        {
            im = &det[i].image;
            dx = det[i].dx;
            dy = det[i].dy;
            break;
        }
    wpt->setImage(im, dx, dy);
}

QString WptMrk::getNamePos(WayPoint* wpt)
{
    QString np;
    switch(wpt->namepos())
    {
    case WayPoint::BottomRight: np = BOTTOMRIGHT; break;
    case WayPoint::Bottom: np = BOTTOM; break;
    case WayPoint::BottomLeft: np = BOTTOMLEFT; break;
    case WayPoint::Left: np = LEFT; break;
    case WayPoint::TopLeft: np = TOPLEFT; break;
    case WayPoint::Top: np = TOP; break;
    case WayPoint::TopRight: np = TOPRIGHT; break;
    case WayPoint::Right: np = RIGHT; break;
    default: np = BOTTOMRIGHT; break;
    }
    return np;
}

void WptMrk::setNamePos(WayPoint* wpt, QString np)
{
    if(np==BOTTOMRIGHT) wpt->setNamepos(WayPoint::BottomRight);
    else if (np == BOTTOM) wpt->setNamepos(WayPoint::Bottom);
    else if(np == BOTTOMLEFT) wpt->setNamepos(WayPoint::BottomLeft);
    else if(np == LEFT) wpt->setNamepos(WayPoint::Left);
    else if(np == TOPLEFT) wpt->setNamepos(WayPoint::TopLeft);
    else if(np == TOP) wpt->setNamepos(WayPoint::Top);
    else if(np == TOPRIGHT) wpt->setNamepos(WayPoint::TopRight);
    else if(np == RIGHT) wpt->setNamepos(WayPoint::Right);
}
