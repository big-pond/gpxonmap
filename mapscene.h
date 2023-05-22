#ifndef MAPSCENE_H
#define MAPSCENE_H

#include <QGraphicsScene>
#include <QHash>
#include <QPixmap>

class Projection;
class DataWdg;
class RubberBand;
class TileReader;
class Gpx;
class CurTrkPos;
class QModelIndex;
class QMenu;
class QAction;

class MapScene : public QGraphicsScene
{
    Q_OBJECT
public:
    enum MapMode {View, Measure, Insert_Pnt, Append_Pnt, Sel_Pnt, Drag_Pnt};//Режимы карты
    explicit MapScene(RubberBand* rb, Projection *p, DataWdg* dw, QObject *parent = 0);
    ~MapScene();

    void setTileReader(TileReader *val);
    void setGpxList(QList<Gpx*>* val) {gpxlist = val;}
    MapMode mapMode(){return mapmode;}
    void setMapMode(MapMode val){mapmode = val;}
    void setViewTileBorders(bool val){drawtileborders = val;}
    bool getViewTileBorders(){return drawtileborders;}

    void pixel_to_metre(const double& X, const double& Y, double& x, double& y);
    void metre_to_pixel(const double& x, const double& y, double& X, double& Y);
    void pixel_to_degree(const double& X, const double& Y, double& lat, double& lon);
    void degree_to_pixel(const double& lat, const double& lon, double& X, double& Y);
    void pixel_to_rad(const double& X, const double& Y, double& lat, double& lon);
    void rad_to_pixel(const double& lat, const double& lon, double& X, double& Y);

    void setLevel(int val);
    int getZ(){return z;}
signals:
    void sendMapsGoToPoint(const double& lat, const double& lon);
public slots:
    void updateTileRect(int x, int y, int z);
    void updateTiles(const QRectF &r);
    double calculateScale(const double &lat);
    void drawCurrentTrackPos(const QModelIndex &current, const QModelIndex &previous);
protected slots:
    void contextMenuAction(QAction *action);
protected:
    RubberBand* rubberband;
    Projection* projection;
    DataWdg* datawdg;
    TileReader* tilereader;
    QHash<QString, QPixmap> tiles;
    QList<Gpx*>* gpxlist;
    Gpx* measure;
    QSize tilesize;
    MapMode mapmode;
    int     z;      //уровень для тайлов [0-20]
    double latmin, latmax, lonmin, lonmax;//минимальные и максимальные координаты карты
    QTransform transf;     //метры карты в единицы сцены (единица сцены соответствует 1 пикселю тайла)
    QTransform backtransf; //единицы  сцены в метры карты

    bool drawtileborders;

    CurTrkPos* curtrkpos;
    QImage im_start;
    QImage im_finish;

    QMenu* cmenu;//контекстное  меню
    QPointF pos_cmenu;
//    QAction* actionDelLastPnt;
//    QAction* actionComplate;
//    QAction* actionCancel;
    QAction* actionFindNearestPnt;
    QAction* actionMapsGoToPoint;
    void createContextMenu();

    void calibrate();
    QRect tileRect(const int &x, const int &y, const int &z);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent);
    void drawTileLayer(QPainter *painter, const QRectF &rect, const int &_z);
    void drawTileBorders(QPainter* painter, const QRectF& rect);
    void drawGpxList(QPainter* painter, const QRectF& rect);  // drawTracks(painter, rect)
    void drawGpx(QPainter *painter, const QRectF &rect, Gpx *gpx);
    void drawRubberBand(QPainter* painter);
    void drawBackground(QPainter *painter, const QRectF &rect);
};

#endif // MAPSCENE_H
