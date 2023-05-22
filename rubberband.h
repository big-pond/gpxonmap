#ifndef RUBBERBAND_H
#define RUBBERBAND_H

#include <QObject>

#include "geopoint.h"
class QColor;
//class GeoPoint;

class RubberBand : public QObject
{
    Q_OBJECT
public:
    enum State {Wait, AppendPoint, InsertPoint, DragPoint, AngleRect, Measure};
    explicit RubberBand(QObject *parent = nullptr);
    ~RubberBand();
    State getState(){return state;}
    void setState(State val);
    int getLineWidth(){return linewidth;}
    QColor&  getColor(){return *color;}
    Qt::PenStyle getStyle(){return style;}
    const GeoPoint* pointAt(int i){return &points.at(i);}
    void setPoint(int i, const double& lat, const double& lon);
    int pointCount(){return point_count;}
    void init(const double &lat, const double &lon);
    void hide();
public slots:
    void mouseMove(const double& lat, const double& lon);
    void mousePress(const double& lat, const double& lon);

signals:
protected:
    State state;
    int linewidth;
    Qt::PenStyle style;
    QColor* color;
    QList<GeoPoint> points;
    int point_count; //Кол-во использующихся точек для рисования резинового контура
};

#endif // RUBBERBAND_H
