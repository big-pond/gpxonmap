#include "mapscene.h"
#include <QPainter>
#include <QModelIndex>
#include <QTableView>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

#include "def.h"
#include "util.h"
#include "projection.h"
#include "ellipsoid.h"
#include "tilereader.h"
#include "datawdg.h"
#include "rubberband.h"
#include "gpx.h"
#include "curtrkpos.h"
#include "trackmodel.h"

#include <QDebug>

MapScene::MapScene(RubberBand *rb, Projection *p, DataWdg *dw, QObject *parent): QGraphicsScene(parent),
    rubberband(rb), projection(p), datawdg(dw), tilereader(nullptr), gpxlist(nullptr), tilesize(256,256),
    mapmode(View), z(1), latmin(0), latmax(0), lonmin(0), lonmax(0),
    drawtileborders(false)
{
    gpxlist = datawdg->getGpxList();
    measure = datawdg->getMeasure();
    curtrkpos = new CurTrkPos();
    addItem(curtrkpos);
    im_start.load(":/resources/start16.png");
    im_finish.load(":/resources/finish16.png");
//        self.datawdg.ui.tvTrack.selectionModel().currentChanged.connect(self.drawCurrentTrackPos)
    connect(datawdg->getTrackTableView()->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), SLOT(drawCurrentTrackPos(QModelIndex, QModelIndex)));

    createContextMenu();
    connect(cmenu, SIGNAL(triggered(QAction*)), this, SLOT(contextMenuAction(QAction*)));
}

MapScene::~MapScene()
{
    if (curtrkpos) delete curtrkpos;
}

void MapScene::setTileReader(TileReader* val)
{
    tilereader = val;
    tilereader->setTiles(&tiles);
    connect(tilereader, SIGNAL(sendUpdateTileRect(int, int, int)), SLOT(updateTileRect(int, int, int)));
}

void MapScene::setLevel(int val)
{
    z = val;
    calibrate();
}

void MapScene::calibrate()
{
    // находим длину окружности по экватору;
    double eqlen = projection->getEllipsoid()->getEquatorLength();
    double eqlen05 = 0.5 * eqlen;
    // находим количество строк тайлов (равно количчеству колонок) для уровня level;
    int rowcol = (1 << z);  // rowcol = (1 << (level-1));
    double h = tilesize.height() * rowcol;
    double w = tilesize.width() * rowcol;
    setSceneRect(0, 0, w, h);
    transf.reset();
    transf.rotate(-90);
    transf.scale(h / eqlen, w / eqlen);
    transf.translate(-eqlen05, eqlen05);
    backtransf = transf.inverted();
}

double MapScene::calculateScale(const double& lat)  // lat - в радианах
{
    double el = projection->getEllipsoid()->getParalleLengthOnLatRad(lat); // # getParalleLengthOnLatRad(lat)
    int rowcol = (1 << z); //  # rowcol = (1 << (level-1));
    return el / (rowcol * tilesize.width());
}

void MapScene::pixel_to_metre(const double& X, const double& Y, double& x, double& y)
{
    backtransf.map(X, Y, &x, &y);
}

void MapScene::metre_to_pixel(const double& x, const double& y, double& X, double& Y)
{
    transf.map(x, y, &X, &Y);
}

void MapScene::pixel_to_degree(const double& X, const double& Y, double& lat, double& lon)
{
    double x, y;
    backtransf.map(X, Y, &x, &y);
    projection->metre_to_geo(x, y, lat, lon, true);
}

void MapScene::degree_to_pixel(const double& lat, const double& lon, double& X, double& Y)
{
    double x, y;
    projection->geo_to_metre(x, y, lat, lon, true);
    transf.map(x, y, &X, &Y);
}

void MapScene::pixel_to_rad(const double& X, const double& Y, double& lat, double& lon)
{
    double x, y;
    backtransf.map(X, Y, &x, &y);
    projection->metre_to_geo(x, y, lat, lon);
}

void MapScene::rad_to_pixel(const double& lat, const double& lon, double& X, double& Y)
{
    double x, y;
    projection->geo_to_metre(x, y, lat, lon);
    transf.map( x, y, &X, &Y);
}

QRect MapScene::tileRect(const int& x, const int& y, const int& z)
{
    int n = this->z - z;
    int mnog = (1 << qAbs(n));
    int th = tilesize.height();
    int tw = tilesize.width();
    QRect r;
    if(n>=0)//тайлы слоя увеличиваются
        r.setRect(x*tw*mnog, y*th*mnog, tw*mnog, th*mnog);
    else
        r.setRect(x*tw/mnog, y*th/mnog, tw/mnog, th/mnog);
    return r;
}

void MapScene::createContextMenu()
{
    cmenu = new QMenu();
//    actionDelLastPnt = new QAction(tr("Удалить последнюю точку"),this);
//    actionComplate = new QAction(tr("Завершить операцию"),this);
//    actionCancel = new QAction(tr("Отменить операцию"),this);
    actionFindNearestPnt = new QAction(tr("Find nearest track point"),this); //Найти ближайшую точку трека
    actionMapsGoToPoint = new QAction(tr("On all maps, move to a point..."),this); //На всех картах перместиться в точку
    cmenu->addAction(actionFindNearestPnt);
    cmenu->addAction(actionMapsGoToPoint);
}

void MapScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent)
{
    pos_cmenu = contextMenuEvent->scenePos();
    cmenu->exec(contextMenuEvent->screenPos());
    QGraphicsScene::contextMenuEvent(contextMenuEvent);
}

void MapScene::contextMenuAction(QAction *action)
{
    if(action==actionFindNearestPnt)
    {
        double /*X,Y,*/ lat, lon;
//        X = pos_cmenu.x();
//        Y = pos_cmenu.y();
//        pixel_to_degree(X, Y, lat, lon);
        pixel_to_degree(pos_cmenu.x(),  pos_cmenu.y(), lat, lon);
        Gpx* gpx = datawdg->getCurrentGpx();
        if(gpx)
        {
            double dp;
            int row = gpx->findNearestTrackPoint(lat, lon, dp);
            int col = 1;
            QModelIndex index = datawdg->getTrackTableView()->currentIndex();
            if (index.isValid())
                col = index.column();
            datawdg->getTrackTableView()->setCurrentIndex(datawdg->getTrackModel()->index(row,col));
        }
    }
    else if(action==actionMapsGoToPoint)
    {
        double lat, lon;
        pixel_to_degree(pos_cmenu.x(),  pos_cmenu.y(), lat, lon);
        emit sendMapsGoToPoint(lat, lon);
    }
//    else if(action==actionComplate) { }
//    else if(action==actionDelLastPnt) {    rsidebar->delLastPoint(); }
//    else if(action==actionCancel) { if(mapmode==M_SEL_PNT) rsb->activate(RubberSidebar::rsAngleRect); }
}


void MapScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    if (!tiles.isEmpty())
        for(int z = 0; z<=this->z; z++)
            drawTileLayer(painter, rect, z);
    if (drawtileborders)
        drawTileBorders(painter, rect);
    drawGpxList(painter, rect);  // drawTracks(painter, rect)
    drawGpx(painter, rect, measure);
    if (rubberband->getState() != RubberBand::Wait)
        drawRubberBand(painter);
}

void MapScene::updateTiles(const QRectF& r)
{
    int startcol = int(r.x() / tilesize.width());
    int startrow = int(r.y() / tilesize.height());
    int colcount = int(r.width() / tilesize.width()) + 1;
    int rowcount = int(r.height() / tilesize.height()) + 1;
    for(int y=startrow; y<startrow + rowcount + 1; y++)
        for(int x = startcol; x<startcol + colcount + 1; x++)
            if ((z>=0) && (y >= 0) && (x >= 0) && (x < (1 << z)) && (y < (1 << z))) // проверка x и y на выход за максимпльные значения строк и колонок;
                tilereader->placeKeyTileToQueue(x, y, z);
}

void MapScene::updateTileRect(int x, int y, int z)
{
    QRectF rect = QRectF(x * tilesize.width(), y * tilesize.height(), tilesize.width(), tilesize.height());
    update(rect);
}

void MapScene::drawTileLayer(QPainter* painter, const QRectF& rect, const int& z)
{
    int n = this->z - z;
    double mnog = (1 << qAbs(n));
    int th = tilesize.height();
    int tw = tilesize.width();
    if (n >= 0)  //тайлы слоя увеличиваются
        mnog = 1. / mnog;
    int startcol = int(rect.x() * mnog / tw);
    int startrow = int(rect.y() * mnog / th);
    int endcol = int((rect.x() + rect.width()) * mnog / tw);
    int endrow = int((rect.y() + rect.height()) * mnog / th);
    int max_row_col = 1 << z;
    if (endcol > max_row_col)
        endcol = max_row_col;
    if (endrow > max_row_col)
        endrow = max_row_col;
    for (int y = startrow; y<=endrow; y++)
        for (int x = startcol; x<=endcol; x++)
        {
            if ((z == this->z) && (y >= 0) && (x >= 0) && (x < (1 << z)) && (y < (1 << z)))
            {
                QString key = QString("%1&%2&%3").arg(x).arg(y).arg(z);  // key = defineKey(x, y, z);
                if (tiles.contains(key))
                {
                    QRect tilerect = tileRect(x, y, z);
                    painter->drawPixmap(tilerect, tiles.value(key));
                }
                else
                {
                    QPen pen;
                    pen.setColor(Qt::red);
                    painter->save();
                    painter->setPen(pen);
                    painter->setOpacity(0.3);
                    QRect tilerect = QRect(x * tw, y * th, tw, th);  // tileRect(x, y, z);
                    painter->drawText(tilerect.x() + tw / 2 - 20, tilerect.y() + th / 2 - 20, "no tile");
                    painter->restore();
                }
            }
        }
}

void MapScene::drawCurrentTrackPos(const QModelIndex& current, const QModelIndex& previous)
{
    if (current.isValid())
    {
        TrackModel* model = datawdg->getTrackModel();
        int row = current.row();
        double lat1 = model->trackPoint(row)->latitude;
        double lon1 = model->trackPoint(row)->longitude;
        double lat2 = lat1;
        double lon2 = lon1;
        if (row < model->rowCount() - 1)
        {
            lat2 = model->trackPoint(row+1)->latitude;
            lon2 = model->trackPoint(row+1)->longitude;
        }
        double X1, Y1, corner;
        degree_to_pixel(lat1, lon1, X1, Y1);
        if ((lat1 == lat2) && (lon1 == lon2))
            corner = 0;
        else
        {
            double X2, Y2;
            degree_to_pixel(lat2, lon2, X2, Y2);
            corner = degrees(atan2((Y2 - Y1), (X2 - X1)) + M_PI_2);
        }
//        qDebug() << X1 << Y1 << corner;
        curtrkpos->setPosition(X1, Y1, corner);
        curtrkpos->setVisible(true);
    }
    else
        curtrkpos->setVisible(false);
}

void MapScene::drawTileBorders(QPainter *painter, const QRectF &rect)
{
    int z = this->z;
    int th = tilesize.height();
    int tw = tilesize.width();
    int startcol = static_cast<int>(rect.x())/tw;
    int startrow = static_cast<int>(rect.y())/th;
    int endcol = static_cast<int>(rect.x()+rect.width())/tw;
    int endrow = static_cast<int>(rect.y()+rect.height())/th;
    QPen pen;
    pen.setColor(Qt::gray);
    pen.setWidth(0);
    painter->save();
    painter->setPen(pen);
    for(int y = startrow; y<=endrow; y++)
        for(int x = startcol; x<=endcol; x++)
        {
            if((y>=0)&&(x>=0)&&(x<(1<<z))&&(y<(1<<z)))
            {
                QRect tilerect = QRect(x*tw, y*th, tw, th);
                painter->drawText(tilerect.x()+tw/4, tilerect.y()+th/2,
                              QString("z=%1 x=%2 y=%3").arg(z).arg(x).arg(y));
                painter->drawRect(tilerect);
            }
        }
    painter->restore();
}

void MapScene::drawGpxList(QPainter *painter, const QRectF &rect)
{
    if (gpxlist->count() > 0)
        for(int i=0; i<gpxlist->count(); i++)
        {
            // bool ispaint = true;
            // if (tr->getLatMax() < latmin) ispaint = false;
            // else if (tr->getLatMin() > latmax) ispaint = false;
            // else if (tr->getLonMax() < lonmin) ispaint = false;
            // else if (tr->getLonMin() > lonmax) ispaint = false;
            // if (ispaint)
            drawGpx(painter, rect, gpxlist->at(i));
        }
}

void MapScene::drawGpx(QPainter* painter, const QRectF& rect, Gpx* gpx)
{
    QPen pen;
    pen.setColor(gpx->getColor());
    pen.setWidthF(gpx->getLineWidth());
//    pen.setStyle(gpx->getStyle());
    painter->save();
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    bool drawspeed = gpx->getDrawSpeed();
    int count = gpx->tptCount();
    int pvisible = gpx->getTrackPointVisible();
    int psize = gpx->getPointSize();
    bool lvisible = gpx->getTrackLineVisible();
    TrackPoint* trp;
    double lat1,lon1,lat2,lon2;
    QPointF p1, p2, psf;
    if (count > 0)
    {
        trp = gpx->trackPointAt(0);
        lat1 = trp->latitude;
        lon1 = trp->longitude;
        degree_to_pixel(lat1, lon1, p1.rx(), p1.ry());
        if (gpx->getDrawStart())
        {
            psf.rx() = p1.x()-1;
            psf.ry() = p1.y()-15;
            painter->drawImage(psf, im_start);
        }
    }
    for (int i=1; i<count; i++)
    {
        trp = gpx->trackPointAt(i);
        lat2 = trp->latitude;
        lon2 = trp->longitude;
        degree_to_pixel(lat2,lon2,p2.rx(),p2.ry());
        if (lvisible)
        {
            if (drawspeed)
            {
                double d, t;
                double v = gpx->getSpeed(i, d, t);//v (км/ч), d (м) и t (сек)
                int idxcolor = (int)(v/10);
                if (idxcolor >=Def::default_speed_colors.count())
                    idxcolor =  Def::default_speed_colors.count()-1;
                pen.setColor(QColor(Def::default_speed_colors.at(idxcolor)));
                painter->setPen(pen);
            }
            painter->drawLine(p1, p2);
        }
        if (pvisible)
            painter->drawEllipse(p2, psize, psize);
        p1 = p2;
    }
    if (count > 1)
    {
        if (gpx->getDrawFinish())
        {
            psf.rx() = p1.x()-1;
            psf.ry() = p1.y()-15;
            painter->drawImage(psf, im_finish);
        }
    }
    if (this->z > 4)  // Draw waypoints
        for(int i=0; i<gpx->wptCount(); i++)
        {
            WayPoint* wpt = gpx->wayPointAt(i);
            degree_to_pixel(wpt->latitude, wpt->longitude, p1.rx(), p1.ry());
            if (rect.contains(p1.rx(), p1.ry()))
            {
                psf.rx() = p1.x() + wpt->dx();
                psf.ry() = p1.y() + wpt->dy();
                if (wpt->image())
                    painter->drawImage(psf, *wpt->image());
                if (this->z > 8)  // Draw text
                {
                    int wdth = painter->fontMetrics().boundingRect(wpt->name()).width();
                    int hght = painter->fontMetrics().height();
                    switch(wpt->namepos())
                    {
                    case WayPoint::TopLeft:
                        psf.rx() = p1.x()- wdth - 6;
                        psf.ry() = p1.y()- hght + 4;
                        break;
                    case WayPoint::Left:
                        psf.rx() = p1.x()- wdth - 6;
                        psf.ry() = p1.y()+ hght/2 - 2;
                        break;
                    case WayPoint::BottomLeft:
                        psf.rx() = p1.x()- wdth - 6;
                        psf.ry() = p1.y()+ hght + 2;
                        break;
                    case WayPoint::Top:
                        psf.rx() = p1.x()- wdth/2;
                        psf.ry() = p1.y()- hght + 4;
                        break;
                    case WayPoint::Bottom:
                        psf.rx() = p1.x()- wdth/2;
                        psf.ry() = p1.y()+ hght + 2;
                        break;
                    case WayPoint::TopRight:
                        psf.rx() = p1.x()+ 8;
                        psf.ry() = p1.y()- hght + 4;
                        break;
                    case WayPoint::Right:
                        psf.rx() = p1.x()+ 8;
                        psf.ry() = p1.y()+ hght/2 - 2;
                        break;
                    case WayPoint::BottomRight:
                        psf.rx() = p1.x()+ 8;
                        psf.ry() = p1.y()+ hght + 2;
                        break;
                    default:
                        psf.rx() = p1.x()+ 8;
                        psf.ry() = p1.y()+ hght;
                        break;
                    }
                    painter->drawText(p1, wpt->name());
                }
            }
        }
    painter->restore();
}

void MapScene::drawRubberBand(QPainter *painter)
{
    QPen pen;
    pen.setColor(rubberband->getColor());
    pen.setWidthF(rubberband->getLineWidth());
    pen.setStyle(rubberband->getStyle());
    painter->save();
    painter->setPen(pen);
    for(int i=0; i<rubberband->pointCount()-1; i++)
    {
        const GeoPoint* tp1 = rubberband->pointAt(i);
        const GeoPoint* tp2 = rubberband->pointAt(i + 1);
        double x1, y1, x2, y2;
        degree_to_pixel(tp1->latitude, tp1->longitude, x1, y1);
        degree_to_pixel(tp2->latitude, tp2->longitude, x2, y2);
        painter->drawLine(QPointF(x1, y1), QPointF(x2, y2));
    }
    painter->restore();
}
