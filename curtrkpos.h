#ifndef CURTRKPOS_H
#define CURTRKPOS_H

#include <QGraphicsPathItem>

class CurTrkPos : public QGraphicsPathItem
{
public:
    explicit CurTrkPos(QGraphicsItem* parent = 0);
    void setPosition(const double& x, const double& y, const double& _corner);
protected:
    void dragLeaveEvent ( QGraphicsSceneDragDropEvent * event );
    void dragMoveEvent ( QGraphicsSceneDragDropEvent * event );
private:
    QPainterPath stoppath;
    QPainterPath movepath;
};

#endif // CURTRKPOS_H
