#include <math.h>
#include <QRegularExpressionValidator>
#include "geocoordedit.h"

#include <QDebug>

const QString GeoCoordEdit::LatD_mask = "09.900000°A";   //Число знаков после точки должно быть равно GeoPoint::DPR
const QString GeoCoordEdit::LatDM_mask  = "09°09.90000'A"; //Число знаков после точки должно быть равно GeoPoint::MPR
const QString GeoCoordEdit::LatDMS_mask   = "09°09'09.90\"A";//Число знаков после точки должно быть равно GeoPoint::SPR
const QString GeoCoordEdit::LonD_mask = "009.900000°A";  //Число знаков после точки должно быть равно GeoPoint::DPR
const QString GeoCoordEdit::LonDM_mask  = "009°09.90000'A";//Число знаков после точки должно быть равно GeoPoint::MPR
const QString GeoCoordEdit::LonDMS_mask   = "009°09'09.90\"A";//Число знаков после точки должно быть равно GeoPoint::SPR

const QRegularExpression GeoCoordEdit::reLatDMS(QString("[0-8 ][0-9]°[0-5 ][0-9]'[0-5 ][0-9]\\.[0-9][0-9 ]{%1}\"[NS]").arg(GeoPoint::SPR-1));
const QRegularExpression GeoCoordEdit::reLatDM (QString("[0-8 ][0-9]°[0-5 ][0-9]\\.[0-9][0-9 ]{%1}'[NS]").arg(GeoPoint::MPR-1));
const QRegularExpression GeoCoordEdit::reLatD  (QString("[0-8 ][0-9]\\.[0-9][0-9 ]{%1}°[NS]").arg(GeoPoint::DPR-1));
const QRegularExpression GeoCoordEdit::reLonDMS(QString("[0-1 ][0-7 ][0-9]°[0-5 ][0-9]'[0-5 ][0-9]\\.[0-9][0-9 ]{%1}\"[WE]").arg(GeoPoint::SPR-1));
const QRegularExpression GeoCoordEdit::reLonDM (QString("[0-1 ][0-7 ][0-9]°[0-5 ][0-9]\\.[0-9][0-9 ]{%1}'[WE]").arg(GeoPoint::MPR-1));
const QRegularExpression GeoCoordEdit::reLonD  (QString("[0-1 ][0-7 ][0-9]\\.[0-9][0-9 ]{%1}°[WE]").arg(GeoPoint::DPR-1));

GeoCoordEdit::GeoCoordEdit(GeoPoint::CoordType ctype, QWidget *parent) : QLineEdit(parent),
    coordtype(ctype), _value(0), hemisphere_for_negative_value(GeoPoint::CSOUTH),
    D_mask(&LatD_mask), DM_mask(&LatDM_mask), DMS_mask(&LatDMS_mask),
    D_regexp(&reLatD), DM_regexp(&reLatDM), DMS_regexp(&reLatDMS)
{
    if (coordtype==GeoPoint::Longitude)
    {
        hemisphere_for_negative_value = GeoPoint::CWEST;
        D_mask = &LonD_mask;
        DM_mask = &LonDM_mask;
        DMS_mask = &LonDMS_mask;
        D_regexp = &reLonD;
        DM_regexp = &reLonDM;
        DMS_regexp = &reLonDMS;
    }

    regexpvalidator =  new QRegularExpressionValidator(this);
    setValidator(regexpvalidator);
    setDisplayFormat(GeoPoint::DegMinSec);
    connect(this, SIGNAL(editingFinished()), this, SLOT(valueUpdate()));
}

GeoCoordEdit::~GeoCoordEdit()
{

}

void GeoCoordEdit::setDisplayFormat(GeoPoint::DispFormat format)
{
    dispformat = format;

    switch(dispformat)
    {
    case GeoPoint::Deg: setInputMask(*D_mask); regexpvalidator->setRegularExpression(*D_regexp); break;
    case GeoPoint::DegMin: setInputMask(*DM_mask); regexpvalidator->setRegularExpression(*DM_regexp); break;
    case GeoPoint::DegMinSec: setInputMask(*DMS_mask); regexpvalidator->setRegularExpression(*DMS_regexp); break;
    default:  setInputMask(*D_mask); regexpvalidator->setRegularExpression(*D_regexp); break;
    }

    updateDisplay();

}
void GeoCoordEdit::setValue(double val)
{
    _value = val;
    updateDisplay();
}

void GeoCoordEdit::valueUpdate()
{
    if(isModified()) _value = strToDeg();
//    qDebug() << isModified() <<"value"<< _value;
}

//gms = 52°49'56.35"N or gms = 52°49.93917'N or gms = 52.8323194°N
double GeoCoordEdit::strToDeg()//ПРОВЕРИТЬ!!!!!!!!! НЕ РАБОТАЕТ ПРИ ОТРИЦАТЕЛЬНЫХ ШИРОТАХ
{
    double deg = 0;
    QString dms = text();
    int idxd = dms.indexOf(GeoPoint::CDEG);
    int idxm = dms.indexOf(GeoPoint::CMIN);
    int idxs = dms.indexOf(GeoPoint::CSEC);
    if(idxd>0)
    {
        deg += dms.section(GeoPoint::CDEG,0,0).trimmed().toDouble();
        if(idxm>0)
            deg += dms.mid(idxd+1, idxm - idxd - 1).trimmed().toDouble()/60;
        if(idxs>0)
            deg += dms.mid(idxm+1, idxs - idxm - 1).trimmed().toDouble()/3600;
    }
    int lenght = dms.length();
    if(lenght>0)
        if (dms[lenght-1]==hemisphere_for_negative_value) deg = - deg;
    return deg;
}

void GeoCoordEdit::updateDisplay()
{
    setText(GeoPoint::degToStr(_value, coordtype, dispformat));
}
