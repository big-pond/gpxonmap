#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "gpxreader.h"

#include "gpx.h"
#include "wptmrk.h"
#include "waypoint.h"

#include <QDebug>

const QLatin1String GpxReader::GPX = QLatin1String("gpx");
const QLatin1String GpxReader::METADATA = QLatin1String("metadata");
const QLatin1String GpxReader::TRK    = QLatin1String("trk");
const QLatin1String GpxReader::WPT    = QLatin1String("wpt");
const QLatin1String GpxReader::BOUNDS = QLatin1String("bounds");
const QLatin1String GpxReader::MINLAT = QLatin1String("minlat");
const QLatin1String GpxReader::MINLON = QLatin1String("minlon");
const QLatin1String GpxReader::MAXLAT = QLatin1String("maxlat");
const QLatin1String GpxReader::MAXLON = QLatin1String("maxlon");
const QLatin1String GpxReader::NAME   = QLatin1String("name");
const QLatin1String GpxReader::EXTENSIONS = QLatin1String("extensions");
const QLatin1String GpxReader::TRKSEG     = QLatin1String("trkseg");
const QLatin1String GpxReader::TRACKEXTENSION = QLatin1String("gpxx:TrackExtension");
const QLatin1String GpxReader::DISPLAYCOLOR = QLatin1String("gpxx:DisplayColor");
const QLatin1String GpxReader::TRKPT = QLatin1String("trkpt");
const QLatin1String GpxReader::LAT   = QLatin1String("lat");
const QLatin1String GpxReader::LON   = QLatin1String("lon");
const QLatin1String GpxReader::ELE   = QLatin1String("ele");
const QLatin1String GpxReader::TIME  = QLatin1String("time");
const QLatin1String GpxReader::DESC  = QLatin1String("desc");
const QLatin1String GpxReader::CMT   = QLatin1String("cmt");
const QLatin1String GpxReader::SRC   = QLatin1String("src");
const QLatin1String GpxReader::SYM   = QLatin1String("sym");
const QLatin1String GpxReader::TYPE  = QLatin1String("type");

GpxReader::GpxReader(QObject *parent)  : QObject(parent),
    time(), lat(0), lon(0), alt(0), name(), marking(), fuel(0), control(false),
    arr(), dep(), gpx(0), reader(0), writer(0)
{
    reader = new QXmlStreamReader();
    writer = new QXmlStreamWriter();
}

GpxReader::~GpxReader()
{
    if(reader!=0) delete reader;
    if(writer!=0) delete writer;
}

void GpxReader::readGpx(QIODevice* dev, Gpx *t)
{
    gpx = t;
    reader->clear();
    reader->setDevice(dev);
    //    reader->addData(dev->readAll());
    reader->readNext();
    while (!reader->atEnd())
    {
        if (reader->isStartElement())
        {
            if (reader->name() == GPX)
                readGpxElement();
            else
                reader->raiseError(QObject::tr("Not a gpx file"));
        }
        else
            reader->readNext();
    }
    gpx->setModified(false);
}

void GpxReader::readGpxElement()
{
    reader->readNext();
    while (!reader->atEnd())
    {
        if (reader->isEndElement())
        {
            reader->readNext();
            break;
        }
        if (reader->isStartElement())
        {
            if (reader->name() == METADATA)
                readMetadataElement();
            else if (reader->name() == TRK)
                readTrkElement();
            else if (reader->name() == WPT)
                readWptElement();
            else
                skipUnknownElement();
        }
        else
            reader->readNext();
    }
}

void GpxReader::readMetadataElement()
{
    reader->readNext();
    while (!reader->atEnd())
    {
        if (reader->isEndElement())
        {
            reader->readNext();
            break;
        }
        if (reader->isStartElement())
        {
            if (reader->name() == BOUNDS)
                readBoundsElement();
            else
                skipUnknownElement();
        }
        else
            reader->readNext();
    }
}

void GpxReader::readBoundsElement()
{
    if(reader->attributes().hasAttribute(MINLAT))
        gpx->setMinLat(reader->attributes().value(MINLAT).toDouble());
    if(reader->attributes().hasAttribute(MINLON))
        gpx->setMinLon(reader->attributes().value(MINLON).toDouble());
    if(reader->attributes().hasAttribute(MAXLAT))
        gpx->setMaxLat(reader->attributes().value(MAXLAT).toDouble());
    if(reader->attributes().hasAttribute(MAXLON))
        gpx->setMaxLon(reader->attributes().value(MAXLON).toDouble());

    reader->readNext();
    while (!reader->atEnd())
    {
        if (reader->isEndElement())
        {
            reader->readNext();
            break;
        }
        if (reader->isStartElement())
        {
            skipUnknownElement();
        }
        else
            reader->readNext();
    }
}

void GpxReader::readTrkElement()
{
    reader->readNext();
    while (!reader->atEnd())
    {
        if (reader->isEndElement())
        {
            reader->readNext();
            break;
        }
        if (reader->isStartElement())
        {
            if (reader->name() == NAME)
            {
                readNameElement();
                gpx->setName(name);
            }
            else if (reader->name() == EXTENSIONS)
                readExtensionsElement();
            else if (reader->name() == TRKSEG)
                readTrkSegElement();
            else
                skipUnknownElement();
        }
        else
            reader->readNext();
    }
}

void GpxReader::readNameElement()
{
    name = reader->readElementText();
    if (reader->isEndElement())
        reader->readNext();
}

void GpxReader::readExtensionsElement()
{
    reader->readNext();
    while (!reader->atEnd())
    {
        if (reader->isEndElement())
        {
            reader->readNext();
            break;
        }
        if (reader->isStartElement())
        {
            if (reader->name() == TRACKEXTENSION)
                readGpxxTrackExtensionElement();
            else
                skipUnknownElement();
        }
        else
            reader->readNext();
    }
}

void GpxReader::readGpxxTrackExtensionElement()
{
    reader->readNext();
    while (!reader->atEnd())
    {
        if (reader->isEndElement())
        {
            reader->readNext();
            break;
        }
        if (reader->isStartElement())
        {
            if (reader->name() == DISPLAYCOLOR)
                readGpxxDisplayColorElement();
            else
                skipUnknownElement();
        }
        else
            reader->readNext();
    }
}

void GpxReader::readGpxxDisplayColorElement()
{
    QString clr  = reader->readElementText();
    if (!clr.isEmpty())
        gpx->setColor(clr);
    if (reader->isEndElement())
        reader->readNext();
}

void GpxReader::readTrkSegElement()
{
    reader->readNext();
    while (!reader->atEnd())
    {
        if (reader->isEndElement())
        {
            reader->readNext();
            break;
        }
        if (reader->isStartElement())
        {
            if (reader->name() == TRKPT)
                readTrkPtElement();
            else
                skipUnknownElement();
        }
        else
            reader->readNext();
    }
}

void GpxReader::readTrkPtElement()
{
    if(reader->attributes().hasAttribute(LAT))
        lat = reader->attributes().value(LAT).toDouble();
    if(reader->attributes().hasAttribute(LON))
        lon = reader->attributes().value(LON).toDouble();

    reader->readNext();
    while (!reader->atEnd())
    {
        if (reader->isEndElement())
        {
            reader->readNext();
            break;
        }
        if (reader->isStartElement())
        {
            if (reader->name() == ELE)
                readEleElement();
            else if (reader->name() == TIME)
                readTimeElement();
            else
                skipUnknownElement();
        }
        else
            reader->readNext();
    }
    gpx->appendTpt(TrackPoint(lat, lon, alt, time));
}

void GpxReader::readEleElement()
{
    alt = reader->readElementText().toDouble();
    if (reader->isEndElement())
        reader->readNext();
}

void GpxReader::readTimeElement()
{
    time = QDateTime::fromString(reader->readElementText(), Qt::ISODate);
    if (reader->isEndElement())
        reader->readNext();
}

void GpxReader::readWptElement()
{
    if(reader->attributes().hasAttribute(LAT))
        lat = reader->attributes().value(LAT).toDouble();
    if(reader->attributes().hasAttribute(LON))
        lon = reader->attributes().value(LON).toDouble();

    reader->readNext();
    while (!reader->atEnd())
    {
        if (reader->isEndElement())
        {
            reader->readNext();
            break;
        }
        if (reader->isStartElement())
        {
            if (reader->name() == ELE)
                readEleElement();
            else if (reader->name() == TIME)
                readTimeElement();
            else if (reader->name() == NAME)
                readNameElement();
            else if (reader->name() == DESC)
                readWptDescElement();
            else
                skipUnknownElement();
        }
        else
            reader->readNext();
    }
    WayPoint wpt(lat, lon, alt, time);
    if (!name.isEmpty()) wpt.setName(name);
    if (!desc.isEmpty()) wpt.setDesc(desc);
    wpt.setMarking(marking);
    if(fuel!=0) wpt.setFuel(fuel);
    if(!namepos.isEmpty()) WptMrk::setNamePos(&wpt, namepos);
    wpt.setControl(control);
    if(!arr.isNull())wpt.setArr(arr);
    if(!dep.isNull())wpt.setDep(dep);
    gpx->appendWpt(wpt);
    name.clear();
    marking.clear();
    fuel = 0;
    desc.clear();
    namepos.clear();
    control = false;
    arr = QDateTime();
    dep = QDateTime();
}

void GpxReader::readWptDescElement()
{
    desc = reader->readElementText();
    parsingDesc();
    if (reader->isEndElement())
        reader->readNext();
}

void GpxReader::skipUnknownElement()
{
    reader->readNext();
    while (!reader->atEnd())
    {
        if (reader->isEndElement())
        {
            reader->readNext();
            break;
        }
        if (reader->isStartElement())
            skipUnknownElement();
        else
            reader->readNext();
    }
}

void GpxReader::parsingDesc()
{
    QStringList desclst = desc.split(";");
    if (desclst.count()==0) return;
    if (wptmrk->isMarking(desclst.at(0)))
    {
        desc.clear();
        marking = desclst.at(0);
        for(int i=1; i<desclst.count(); i++)
        {
            QStringList kvlst = desclst.at(i).split("=");
            if(kvlst.count()==0) continue;
            QString key, val;
            key = kvlst.at(0).trimmed();
            if(kvlst.count()==2)
                val = kvlst.at(1).trimmed();
            if(key.compare(WptMrk::CONTROL)==0)
                control = (val=="yes");
            else if (key.compare(WptMrk::FUEL)==0)
                fuel = val.toDouble();
            else if (key.compare(WptMrk::TANKFUEL)==0)
                fuel = val.toDouble();
            else if (key.compare(WptMrk::LABEL)==0)
                namepos =  val;
            else if (key.compare(WptMrk::ARR)==0)
                arr =  QDateTime::fromString(val, Qt::ISODate);
            else if (key.compare(WptMrk::DEP)==0)
                dep =  QDateTime::fromString(val, Qt::ISODate);
            else
                desc = key;
        }
    }
}

void GpxReader::makeDesc(WayPoint *p)
{
    desc = QString("%1;").arg(p->marking());
    if(p->fuel()>0)
    {
        QString pf = WptMrk::FUEL;
        if((p->marking()==WptMrk::START)||(p->marking()==WptMrk::FINISH))
            pf = WptMrk::TANKFUEL;
        desc += QString("%1=%2;").arg(pf).arg(p->fuel(),0,'f',2);
    }
    if(p->control())
    {
        desc += QString("%1=yes;").arg(WptMrk::CONTROL);
    }
    if(p->namepos()!= WayPoint::BottomRight)
    {
        QString np = WptMrk::getNamePos(p);
        desc += QString("%1=%2;").arg(WptMrk::LABEL).arg(np);
    }
    if(p->arr()!=p->time || p->dep()!=p->time)
    {
        desc += QString("%1=%2;").arg(WptMrk::ARR).arg(p->arr().toString(Qt::ISODate));
        desc += QString("%1=%2;").arg(WptMrk::DEP).arg(p->dep().toString(Qt::ISODate));
    }
    if(!p->desc().isEmpty())
        desc += p->desc();
    if(!desc.isEmpty())
        if(desc[desc.size()-1]==';')
            desc.resize(desc.size()-1);//Убираем последниюю ';'
}

void GpxReader::writeGPX(QIODevice* dev, Gpx* t)
{
    gpx = t;
    writer->setDevice(dev);
    writer->setAutoFormatting(true);
    writer->writeStartDocument();

    writer->writeStartElement("gpx");
    writer->writeAttribute("xmlns", "http://www.topografix.com/GPX/1/0");
    writer->writeAttribute("xmlns:gpxx", "http://www.garmin.com/xmlschemas/GpxExtensions/v3");
    writer->writeAttribute("version", "1.0");
    writer->writeAttribute("creator", "gpxonmap");
    writer->writeStartElement("metadata");
        writer->writeStartElement("bounds");
        writer->writeAttribute("minlat", QString::number(gpx->getMinLat(),'f',8));
        writer->writeAttribute("minlon", QString::number(gpx->getMinLon(),'f',8));
        writer->writeAttribute("maxlat", QString::number(gpx->getMaxLat(),'f',8));
        writer->writeAttribute("maxlon", QString::number(gpx->getMaxLon(),'f',8));
        writer->writeEndElement();
    writer->writeEndElement();
    writer->writeStartElement("trk");
        writer->writeTextElement("name", gpx->getName());
        writer->writeStartElement("extensions");
            writer->writeStartElement("gpxx:TrackExtension");
                writer->writeTextElement("gpxx:DisplayColor", gpx->getColor());
            writer->writeEndElement(); //gpxx:TrackExtension
        writer->writeEndElement(); //extensions
        writer->writeStartElement("trkseg");
        for (int i = 0; i < gpx->tptCount(); ++i)
        {
            TrackPoint* p = gpx->trackPointAt(i);
            writer->writeStartElement("trkpt");
            writer->writeAttribute("lat", QString::number(p->latitude,'f',8));
            writer->writeAttribute("lon", QString::number(p->longitude,'f',8));
            writer->writeTextElement("ele", QString::number(p->altitude,'f',2));
            writer->writeTextElement("time", p->time.toString(Qt::ISODate));
            writer->writeEndElement();//trkpt
        }
        writer->writeEndElement(); //trkseg
    writer->writeEndElement(); //trk
    for(int i=0; i<gpx->wptCount(); i++)
    {
        WayPoint* p = gpx->wayPointAt( i);
        writer->writeStartElement("wpt");
        writer->writeAttribute("lat", QString::number(p->latitude,'f',8));
        writer->writeAttribute("lon", QString::number(p->longitude,'f',8));
        writer->writeTextElement("ele", QString::number(p->altitude,'f',2));
        writer->writeTextElement("time", p->time.toString(Qt::ISODate));
        if(!p->name().isEmpty())writer->writeTextElement("name", p->name());
        if(!p->cmt().isEmpty()) writer->writeTextElement("cmt", p->cmt());
        if(!p->src().isEmpty()) writer->writeTextElement("src", p->src());
        if(!p->sym().isEmpty()) writer->writeTextElement("sym", p->sym());
        if(!p->type().isEmpty())writer->writeTextElement("type", p->type());
        if(p->isMarking())
        {
            makeDesc(p); //формируем пременную desc
            writer->writeTextElement("desc", desc);
        }
        else
        {
            if(!p->desc().isEmpty())
                writer->writeTextElement("desc", p->desc());
        }
        writer->writeEndElement();//wpt
    }
    writer->writeEndDocument();
}
