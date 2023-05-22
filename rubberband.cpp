#include "rubberband.h"
#include <QColor>
//#include "geopoint.h"

RubberBand::RubberBand(QObject *parent)
    : QObject{parent}, state(Wait), linewidth(0), style(Qt::DotLine)
{
    color = new QColor(Qt::red);
    for(int i=0; i<5; i++)
        points << GeoPoint();
    point_count = 0;
}

RubberBand::~RubberBand()
{
    delete color;
}

void RubberBand::setState(State val)
{
    state = val;
    switch(val)
    {
    case Wait:        point_count = 0; break;
    case AppendPoint: point_count = 2; break;
    case InsertPoint: point_count = 3; break;
    case DragPoint:   point_count = 3; break;
    case AngleRect:   point_count = 0; break;
    default:          point_count = 0; break;
    }
}

void RubberBand::setPoint(int i, const double &lat, const double &lon)
{
    points[i].latitude = lat;
    points[i].longitude = lon;
}

void RubberBand::init(const double& lat, const double& lon)
{
    for (int i=0; i<point_count; i++)
    {
        points[i].latitude = lat;
        points[i].longitude = lon;
    }
}

void RubberBand::hide()
{
    state = Wait;
    point_count = 0;
}

void RubberBand::mouseMove(const double &lat, const double &lon)
{
    if (state == RubberBand::Wait)
        return;
    else if (state == RubberBand::AppendPoint)
    {
        points[1].latitude = lat;
        points[1].longitude = lon;
    }
    else if (state == RubberBand::InsertPoint || state == RubberBand::DragPoint)
    {
        points[1].latitude = lat;
        points[1].longitude = lon;
    }
    else if (state == RubberBand::AngleRect)
        return;
    else
        return;
}

void RubberBand::mousePress(const double &lat, const double &lon)
{
    if (state == RubberBand::Wait)
        return;
    else if (state == RubberBand::AppendPoint || state == RubberBand::InsertPoint)
    {
            points[0].latitude = lat;
            points[0].longitude = lon;
            points[1].latitude = lat;
            points[1].longitude = lon;
    }
    else if (state == RubberBand::DragPoint)
            return;
    else if (state == RubberBand::AngleRect)
        return;
}
