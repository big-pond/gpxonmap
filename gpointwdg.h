#ifndef GPOINTWDG_H
#define GPOINTWDG_H

#include <QWidget>
#include "geopoint.h"

class QLabel;
class QDoubleSpinBox;
class QComboBox;
class GeoCoordEdit;
class QGridLayout;

class GPointWdg : public QWidget
{
    Q_OBJECT
public:
    explicit GPointWdg(QWidget *parent = 0);
    ~GPointWdg();
    void setGeoCoordinate(const double& lat, const double& lon, const double& alt,
                          GeoPoint::DispFormat format);
    void setGeoCoordinate(const GeoPoint& gc, GeoPoint::DispFormat format);
    GeoPoint getGeoCoordinate();
    void getGeoCoordinate(double& lat, double& lon);
    void getGeoCoordinate(double& lat, double& lon, double& alt);
    void setAltVisible(bool val);

signals:

public slots:
protected slots:
    void changeCoordDispFormat(int);
protected:
    QLabel* lbLat;
    QLabel* lbLon;
    QLabel* lbAlt;
    QComboBox* cbFormat;
    GeoCoordEdit* gcLat;
    GeoCoordEdit* gcLon;
    QDoubleSpinBox* sbAlt;
    QGridLayout* glayout;

};

#endif // GPOINTWDG_H
