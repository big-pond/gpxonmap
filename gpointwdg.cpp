#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include "gpointwdg.h"

#include "geocoordedit.h"

GPointWdg::GPointWdg(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    cbFormat = new QComboBox();
    cbFormat->addItem(GeoPoint::getFormat(GeoPoint::Deg), GeoPoint::Deg);
    cbFormat->addItem(GeoPoint::getFormat(GeoPoint::DegMin), GeoPoint::DegMin);
    cbFormat->addItem(GeoPoint::getFormat(GeoPoint::DegMinSec), GeoPoint::DegMinSec);

    lbLat = new QLabel(tr("Широта"));
    lbLon = new QLabel(tr("Долгота"));
    lbAlt = new QLabel(tr("Высота"));
    lbAlt->setToolTip(tr("Высота над уровнем моря"));

    gcLat = new GeoCoordEdit(GeoPoint::Latitude);
    gcLon = new GeoCoordEdit(GeoPoint::Longitude);
    sbAlt = new QDoubleSpinBox;
    sbAlt->setRange(-500, 10000);
    sbAlt->setSuffix(tr(" м"));

    glayout = new QGridLayout();
//    gl->setSpacing(0);
    int r=0;
    glayout->addWidget(new QLabel(tr("Формат")),r,0);
    glayout->addWidget(cbFormat,r,1);
    r++;
    glayout->addWidget(lbLat,r,0);
    glayout->addWidget(gcLat,r,1);
    r++;
    glayout->addWidget(lbLon,r,0);
    glayout->addWidget(gcLon,r,1);
    r++;

    glayout->addWidget(lbAlt,r,0);
    glayout->addWidget(sbAlt,r,1);
    r++;
    setLayout(glayout);

    connect(cbFormat, SIGNAL(currentIndexChanged(int)),SLOT(changeCoordDispFormat(int)));
}

GPointWdg::~GPointWdg()
{

}

void GPointWdg::setGeoCoordinate(const double& lat, const double& lon, const double& alt, GeoPoint::DispFormat format)
{
    gcLat->setValue(lat);
    gcLon->setValue(lon);
    sbAlt->setValue(alt);
    int idx = -1;
    for(int i=0; i<cbFormat->count(); i++)
        if(cbFormat->itemData(i).toInt()==format)
        {
            idx = i;
            break;
        }
    cbFormat->setCurrentIndex(idx);
    gcLat->setDisplayFormat(format);
    gcLon->setDisplayFormat(format);
}

void GPointWdg::setGeoCoordinate(const GeoPoint& gc, GeoPoint::DispFormat format)
{
    setGeoCoordinate(gc.latitude, gc.longitude, gc.altitude, format);
}

void GPointWdg::changeCoordDispFormat(int /*idx*/)
{
    GeoPoint::DispFormat format = static_cast<GeoPoint::DispFormat>(cbFormat->currentData().toInt());

    gcLat->setDisplayFormat(format);
    gcLon->setDisplayFormat(format);
}

void GPointWdg::setAltVisible(bool val)
{
    lbAlt->setVisible(val);
    sbAlt->setVisible(val);
}

GeoPoint GPointWdg::getGeoCoordinate()
{
    return GeoPoint(gcLat->value(), gcLon->value(), sbAlt->value());
}

void GPointWdg::getGeoCoordinate(double& lat, double& lon)
{
    lat = gcLat->value();
    lon = gcLon->value();
}

void GPointWdg::getGeoCoordinate(double& lat, double& lon, double& alt)
{
    lat = gcLat->value();
    lon = gcLon->value();
    alt = sbAlt->value();

}

