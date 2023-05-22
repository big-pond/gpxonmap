#include "mapview.h"
#include <QScrollBar>
#include <QMouseEvent>
#include <QModelIndex>
#include <QTableView>
#include <QBitmap>
#include "util.h"
#include "gpx.h"
#include "rubberband.h"
#include "mapscene.h"
#include "datawdg.h"

MapView::MapView(DataWdg* dw, QWidget *parent)
    : QGraphicsView(parent ), datawdg(dw)
{
    rubberband = new RubberBand(this);
    setMouseTracking(true);
    setCursor(Qt::CrossCursor);
    horizontalScrollBar()->setCursor(Qt::ArrowCursor);
    verticalScrollBar()->setCursor(Qt::ArrowCursor);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

/*  Qt 6.2.0
    addpcursor = new QCursor(QBitmap(":/resources/cursaddp.bmp"), QBitmap(":/resources/cursaddp.bmp"), 14, 25);
    dragpcursor = new QCursor(QBitmap(":/resources/cursmovep.bmp"), QBitmap(":/resources/cursmovep.bmp"), 14, 25);
    measurecursor = new QCursor(QBitmap(":/resources/cursmeas.bmp"), QBitmap(":/resources/cursmeas.bmp"), 15, 25);
*/
    QPixmap cursap(":/resources/cursaddp.bmp");
    cursap.setMask(cursap);
    addpcursor = new QCursor(cursap,14,25);
    QPixmap cursmp(":/resources/cursmovep.bmp");
    cursmp.setMask(cursmp);
    dragpcursor = new QCursor(cursmp,14,25);
    QPixmap cursmeas(":/resources/cursmeas.bmp");
    cursmeas.setMask(cursmeas);
    measurecursor = new QCursor(cursmeas,15,25);

    measurepoint = new QImage(":/resources/scirc.png");
    connect(datawdg->getTrackTableView()->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
                                                              SLOT(trackCurrentRowChanged(QModelIndex, QModelIndex)));
}

MapView::~MapView()
{
    delete  addpcursor;
    delete  dragpcursor;
    delete  measurecursor;
    delete  measurepoint;
}

QRectF MapView::getDisplayedMapRect()
{
    QPointF topleft = mapToScene(rect().topLeft());
    QPointF botright = mapToScene(rect().bottomRight());
    return QRectF(topleft, botright);
}

void MapView::mousePressEvent(QMouseEvent *event)
{
    QPointF p = mapToScene(event->pos());
    MapScene* mapscene = dynamic_cast<MapScene*>(scene());
    double lat, lon;
    mapscene->pixel_to_degree(p.x(), p.y(), lat, lon);
    if (mapscene->mapMode() == MapScene::Append_Pnt)
    {
        rubberband->mousePress(lat, lon);
        datawdg->appendTrackPoint(lat, lon);
    }
    else if (mapscene->mapMode() == MapScene::Insert_Pnt)
    {
        rubberband->mousePress(lat, lon);
        datawdg->insertTrackPoint(lat, lon);
    }
    else if (mapscene->mapMode() == MapScene::Drag_Pnt)
    {
        rubberband->mousePress(lat, lon);
        datawdg->changePointCoordinates(lat, lon);
    }
    else if (mapscene->mapMode() == MapScene::Measure)
    {
        rubberband->mousePress(lat, lon);
        datawdg->getMeasure()->appendTpt(TrackPoint(lat, lon));
        double s = datawdg->getMeasure()->getLength();
        WayPoint wp(lat, lon);
        wp.setName(QString::number(s*0.001,'f',3));
        wp.setImage(measurepoint, -7, -7);
        datawdg->getMeasure()->appendWpt(wp);
    }
    else if (mapscene->mapMode() == MapScene::Sel_Pnt)
        rubberband->mousePress(lat, lon);
    else
    {
        if (event->button() == Qt::LeftButton)
            setDragMode(QGraphicsView::ScrollHandDrag);
    }
    QGraphicsView::mousePressEvent(event);
}

void MapView::mouseReleaseEvent(QMouseEvent *event)
{
    if (dragMode()==QGraphicsView::ScrollHandDrag)
    {
        setDragMode(QGraphicsView::NoDrag);
        MapScene* mapscene = dynamic_cast<MapScene*>(scene());
        if (mapscene)
        {
            QPointF topleft = mapToScene(rect().topLeft());
            QPointF botright = mapToScene(rect().bottomRight());
            QRectF rf(topleft,botright);
            mapscene->updateTiles(rf);
        }
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void MapView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
    QPointF p = mapToScene(event->pos());
    emit sendMouseMoveXY(p.x(), p.y());
    MapScene* mapscene = dynamic_cast<MapScene*>(scene());
    if ((mapscene->mapMode() == MapScene::Append_Pnt)
            || (mapscene->mapMode() == MapScene::Insert_Pnt)
            || (mapscene->mapMode() == MapScene::Drag_Pnt)
            || (mapscene->mapMode() == MapScene::Measure)
            || (mapscene->mapMode() == MapScene::Sel_Pnt))
    {
        double  lat, lon;
        mapscene->pixel_to_degree(p.x(), p.y(), lat, lon);
        rubberband->mouseMove(lat, lon);
        mapscene->update();  // map.update(rubberband.rect()); //остаются полосы
    }
    double x, y;
    mapscene->pixel_to_rad(p.x(), p.y(), x, y);
        //# if(cursreflect.isVisible())
        //#     emit sendCursorReflection(x, y, projection.getDatumId());
    double scale = mapscene->calculateScale(x);
    if (GeoPoint::display_format ==GeoPoint::Pixel)
    {
            x = p.x();
            y = p.y();
    }
    else if (GeoPoint::display_format == GeoPoint::Metre)
        mapscene->pixel_to_metre(p.x(), p.y(), x, y);
    else if (GeoPoint::display_format == GeoPoint::Deg
             || GeoPoint::display_format == GeoPoint::DegMin
             || GeoPoint::display_format == GeoPoint::DegMinSec
             || GeoPoint::display_format == GeoPoint::Rad)
    {
        x = degrees(x);
        y = degrees(y);
    }
    emit  sendCoordinates(x, y, scale); // посылаются координаты в датуме карты
}

void MapView::appendTrackPointOnClick(bool turn_on)
{
    MapScene* mapscene = dynamic_cast<MapScene*>(scene());
    if (turn_on)
    {
        Gpx* gpx = datawdg->getCurrentGpx();
        if (gpx)
        {
            setCursor(*addpcursor);
            mapscene->setMapMode(MapScene::Append_Pnt);
            rubberband->setState(RubberBand::AppendPoint);
            double lat=0, lon=0;
            if (gpx->tptCount() > 0)
            {
                lat = gpx->trackPointAt(gpx->tptCount()-1)->latitude;  // points.append(gpx.trackpoints[-1])
                lon = gpx->trackPointAt(gpx->tptCount()-1)->longitude; // points.append(gpx.trackpoints[-1])
            }
            rubberband->init(lat, lon);
        }
    }
    else
    {
        setCursor(Qt::CrossCursor);
        mapscene->setMapMode(MapScene::View);
        rubberband->hide();
        mapscene->update(); //  убираем линии rubberband
    }
}

void MapView::insertTrackPointOnClick(bool turn_on)
{
    MapScene* mapscene = dynamic_cast<MapScene*>(scene());
    if (turn_on)
    {
        Gpx* gpx = datawdg->getCurrentGpx();
        if (gpx)
        {
            setCursor(*addpcursor);
            mapscene->setMapMode(MapScene::Insert_Pnt);
            rubberband->setState(RubberBand::InsertPoint);
            double lat=0, lon=0;
            int row = datawdg->getCurrentTrackPointRow();
            if (row>=0)
            {
                lat = gpx->trackPointAt(row)->latitude;
                lon = gpx->trackPointAt(row)->longitude;
            }
            rubberband->init(lat, lon);
            if (row>0)
                rubberband->setPoint(0, gpx->trackPointAt(row-1)->latitude, gpx->trackPointAt(row-1)->longitude);
        }
    }
    else
    {
        setCursor(Qt::CrossCursor);
        mapscene->setMapMode(MapScene::View);
        rubberband->hide();
        mapscene->update(); //  убираем линии rubberband
    }
}

void MapView::dragTrackPoint(bool turn_on)
{
    MapScene* mapscene = dynamic_cast<MapScene*>(scene());
    if (turn_on)
    {
        Gpx* gpx = datawdg->getCurrentGpx();
        if (gpx)
        {
            setCursor(*dragpcursor);
            mapscene->setMapMode(MapScene::Drag_Pnt);
            rubberband->setState(RubberBand::DragPoint);
            double lat=0, lon=0;
            int row = datawdg->getCurrentTrackPointRow();
            if (row>=0)
            {
                lat = gpx->trackPointAt(row)->latitude;
                lon = gpx->trackPointAt(row)->longitude;
            }
            rubberband->init(lat, lon);
            if (row>0)
            {
                rubberband->setPoint(0, gpx->trackPointAt(row-1)->latitude, gpx->trackPointAt(row-1)->longitude);
                rubberband->setPoint(2, gpx->trackPointAt(row+1)->latitude, gpx->trackPointAt(row+1)->longitude);
            }
        }
    }
    else
    {
        setCursor(Qt::CrossCursor);
        mapscene->setMapMode(MapScene::View);
        rubberband->hide();
        mapscene->update(); //  убираем линии rubberband
    }
}

void MapView::measure(bool turn_on)
{
    MapScene* mapscene = dynamic_cast<MapScene*>(scene());
    if (turn_on)
    {
        setCursor(*measurecursor);
        mapscene->setMapMode(MapScene::Measure);
        rubberband->setState(RubberBand::Measure);
        double lat=0, lon=0;
        Gpx* mr = datawdg->getMeasure();
        if (mr->tptCount() > 0) // Если продолжаем измерять после остановки измерения
        {
            lat = mr->trackPointAt(mr->tptCount()-1)->latitude;
            lon = mr->trackPointAt(mr->tptCount()-1)->longitude;
            rubberband->init(lat, lon);
        }
    }
    else
    {
        setCursor(Qt::CrossCursor);
        mapscene->setMapMode(MapScene::View);
        rubberband->hide();
        mapscene->update(); //  убираем линии rubberband
    }

}

void MapView::mapSceneUpdate()
{
    scene()->update();
}

void MapView::trackCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if (current.isValid())
    {
        MapScene* mapscene = dynamic_cast<MapScene*>(scene());
        Gpx* gpx = datawdg->getCurrentGpx();
        int row = current.row();
        int rowprev = row;
        if (row > 0)
            rowprev = row - 1;
        int rownext = row;
        if (row < current.model()->rowCount() - 1)
            rownext = row + 1;
        //if (mapscene->mapMode() == MapScene::Append_Pnt) {} // для режима Map.APPEND_PNT ничего делать не надо
        if (mapscene->mapMode() == MapScene::Insert_Pnt)
        {
            rubberband->setPoint(0, gpx->trackPointAt(rowprev)->latitude, gpx->trackPointAt(rowprev)->longitude);
            rubberband->setPoint(1, gpx->trackPointAt(row)->latitude, gpx->trackPointAt(row)->longitude);
            rubberband->setPoint(2, gpx->trackPointAt(row)->latitude, gpx->trackPointAt(row)->longitude);
        }
        else if (mapscene->mapMode() == MapScene::Drag_Pnt)
        {
            rubberband->setPoint(0, gpx->trackPointAt(rowprev)->latitude, gpx->trackPointAt(rowprev)->longitude);
            rubberband->setPoint(1, gpx->trackPointAt(row)->latitude, gpx->trackPointAt(row)->longitude);
            rubberband->setPoint(2, gpx->trackPointAt(rownext)->latitude, gpx->trackPointAt(rownext)->longitude);
        }
        mapscene->update();
    }
}
