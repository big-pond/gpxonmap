#include "curtrkpos.h"
#include <QPen>
#include <QBrush>

CurTrkPos::CurTrkPos(QGraphicsItem* parent) : QGraphicsPathItem(parent)
{
    setPen(QPen(Qt::blue));
    setBrush(QBrush(Qt::red));
    stoppath.addEllipse(QPointF(0,0), 4, 4);
    QPolygonF poly;
    poly << QPointF(0,0) << QPointF(-5, 5) << QPointF(0,-10) << QPointF(5,5);
    movepath.addPolygon(poly);
    setPath(stoppath);
    setTransformOriginPoint(0, 0);
    setVisible(false);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

void CurTrkPos::setPosition(const double& x, const double& y, const double& corner)
{
    setPos(x, y);

    if(corner==0)
        setPath(stoppath);
    else
    {
        setPath(movepath);
        setRotation(corner);
    }
}

void CurTrkPos::dragMoveEvent ( QGraphicsSceneDragDropEvent * event )
{
    QGraphicsPathItem::dragMoveEvent(event);
}

void CurTrkPos::dragLeaveEvent ( QGraphicsSceneDragDropEvent * event )
{
    QGraphicsPathItem::dragLeaveEvent(event);
}
