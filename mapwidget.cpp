#include "mapwidget.h"

#include <QVBoxLayout>
#include <QTimer>
//#include <QPolygonF>

#include "def.h"
#include "util.h"
#include "datawdg.h"
#include "mapdescription.h"
#include "projection.h"
#include "tilereader.h"
#include "mapview.h"
#include "levelwdg.h"
#include "scalewdg.h"
#include "mapscene.h"
#include "geopoint.h"

//#include <QDebug>

MapWidget::MapWidget(DataWdg* dw, MapDescription* md, QMap<int, Projection*>* prjs, QWidget *parent)
    : QWidget{parent}, datawdg(dw), mapdescr(md), projection(nullptr)
{
    setWindowTitle(mapdescr->getName());
    mapView = new MapView(datawdg, this);
    QVBoxLayout* vbl = new QVBoxLayout(this);
    vbl->setContentsMargins(0, 0, 0, 0);
    vbl->addWidget(mapView);
    setLayout(vbl);
    levelwdg = new LevelWdg(this);
    levelwdg->move(6, 6);
    scalewdg = new ScaleWdg(this);
    scalewdg->move(6, 40);
    scalewdg->setScale(611);
    if (mapdescr->getEllipsoid() == Def::S6378137 && mapdescr->getProjection() == Def::MERCATOR)
        projection = prjs->value(Projection::MERC_S6378137);
    else if (mapdescr->getEllipsoid() == Def::S6371000 && mapdescr->getProjection() == Def::MERCATOR)
        projection = prjs->value(Projection::MERC_S6371000);
    else if (mapdescr->getEllipsoid() == Def::KRASOVSKY)
        projection = prjs->value(Projection::GK_KRASOVSKY);
    else if (mapdescr->getEllipsoid() == Def::WGS84 && mapdescr->getProjection() == Def::MERCATOR)
        projection = prjs->value(Projection::MERC_WGS84);
    mapScene = new MapScene(mapView->getRubberband(), projection, datawdg, this);
    tilereader = new TileReader(QString("cache"), this);
    tilereader->setCacheSize(mapdescr->getCacheSize());
    tilereader->setTimeout(mapdescr->getTimeout());
    tilereader->setUrlBase(mapdescr->getUrlbase());
    tilereader->setCacheloadcontrol(mapdescr->getTileSource());
    mapScene->setTileReader(tilereader);
    mapView->setScene(mapScene);
    connect(levelwdg, SIGNAL(sendLevel(int)), SLOT(gotoLayer(int)));
    setGeometry(QRect(0, 0, mapdescr->getWidth(), mapdescr->getHeight()));
//    qDebug() << mapdescr->getLat()<< mapdescr->getLon();
    QTimer::singleShot(500, this, SLOT(doMapGoTo())); //Задержка, иначе неверно определяется центр карты
}

int MapWidget::getLevel()
{
    return levelwdg->getLevel();
}

void MapWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (mapScene)
    {
        QRectF displayed_rect = getDisplayedMapRect();
        mapScene->updateTiles(displayed_rect);
    }
}

void MapWidget::mapGoTo(const GeoPoint& center, int z)
{
    if (z >= 0)
    {
        mapScene->setLevel(z);
        levelwdg->setLevel(z);
        scalewdg->setScale(mapScene->calculateScale(radians(center.latitude)));
    }
    double X, Y;
    mapScene->degree_to_pixel(center.latitude, center.longitude, X, Y);
    mapView->centerOn(X, Y);
    QRectF r = mapView->getDisplayedMapRect();
    mapScene->updateTiles(r);
}

void MapWidget::doMapGoTo()
{
    mapGoTo(GeoPoint(mapdescr->getLat(), mapdescr->getLon()), mapdescr->getZ());
}

void MapWidget::gotoLayer(int z)
{
        double lat, lon;
        getMapPosition(lat, lon);
        mapScene->setLevel(z);
        scalewdg->setScale(mapScene->calculateScale(radians(lat)));
        double X, Y;
        mapScene->degree_to_pixel(lat, lon, X, Y);
        mapView->centerOn(X, Y);
        QRectF r = mapView->getDisplayedMapRect();
        mapScene->updateTiles(r);
}

QRectF MapWidget::getDisplayedMapRect()
{
    QPointF topleft = mapView->mapToScene(mapView->rect().topLeft());
    QPointF botright = mapView->mapToScene(mapView->rect().bottomRight());
    return QRectF(topleft, botright);
}

void MapWidget::getMapPosition(double& lat, double& lon)
{
//    QRect r = mapView->rect();
    QPolygonF p = mapView->mapToScene(mapView->rect());
    double X = (p[2].x() - p[0].x()) * 0.5 + p[0].x();
    double Y = (p[2].y() - p[0].y()) * 0.5 + p[0].y();
    mapScene->pixel_to_degree(X, Y, lat, lon);
}

void MapWidget::mapGoToTopLeftPosition(const GeoPoint &gp, int z)
{
    if (z >= 0)
    {
        mapScene->setLevel(z);
        levelwdg->setLevel(z);
        scalewdg->setScale(mapScene->calculateScale(radians(gp.latitude)));
    }
    QPolygonF p = mapView->mapToScene(mapView->rect());
    double w_2 = (p[2].x() - p[0].x()) * 0.5;
    double h_2 = (p[2].y() - p[0].y()) * 0.5;
    double X, Y;
    mapScene->degree_to_pixel(gp.latitude, gp.longitude, X, Y);
    mapView->centerOn(X+w_2, Y+h_2);
    QRectF r = mapView->getDisplayedMapRect();
    mapScene->updateTiles(r);

}

void MapWidget::getMapTopLeftPosition(double &lat, double &lon)
{
    QRect r = mapView->rect();
    QPolygonF p = mapView->mapToScene(r);
    mapScene->pixel_to_degree(p[0].x(), p[0].y(), lat, lon);
}

QString MapWidget::mapProperty()
{
    return  tr("Name: %1\nDatum: %2\n Projection: %3").arg(mapdescr->getName())
            .arg(mapdescr->getEllipsoid()).arg(mapdescr->getProjection());
}
