#ifndef GEOCOORDEDIT_H
#define GEOCOORDEDIT_H

#include <QLineEdit>
#include <QRegularExpression>
#include "geopoint.h"

class QRegularExpressionValidator;

class GeoCoordEdit : public QLineEdit
{
    Q_OBJECT
public:
    static const QString LatDMS_mask;
    static const QString LatDM_mask;
    static const QString LatD_mask;
    static const QString LonDMS_mask;
    static const QString LonDM_mask;
    static const QString LonD_mask;

    static const QRegularExpression reLatDMS;
    static const QRegularExpression reLatDM;
    static const QRegularExpression reLatD;
    static const QRegularExpression reLonDMS;
    static const QRegularExpression reLonDM;
    static const QRegularExpression reLonD;


//    static QString degToStr(const double& val, GPoint::CoordType type, GPoint::DispFormat format);

    explicit GeoCoordEdit(GeoPoint::CoordType ctype,QWidget *parent = 0);
    ~GeoCoordEdit();
    void setDisplayFormat(GeoPoint::DispFormat format);
    void setValue(double val);
    double value(){return _value;}

signals:

public slots:
protected slots:
    void valueUpdate();
protected:
    void updateDisplay();
    double strToDeg();

    GeoPoint::DispFormat dispformat;
    GeoPoint::CoordType coordtype;
    QRegularExpressionValidator* regexpvalidator;
    double _value;

    QChar hemisphere_for_negative_value;

    const QString* D_mask;
    const QString* DM_mask;
    const QString* DMS_mask;

    const QRegularExpression* D_regexp;
    const QRegularExpression* DM_regexp;
    const QRegularExpression* DMS_regexp;
};

#endif // GEOCOORDEDIT_H
