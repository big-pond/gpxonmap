#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>

class DataWdg;
class MapDescription;
class Projection;
class TileReader;
class MapView;
class LevelWdg;
class ScaleWdg;
class MapScene;
class GeoPoint;

class MapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapWidget(DataWdg* dw, MapDescription* md, QMap<int, Projection *> *prjs, QWidget *parent = nullptr);

    MapView* getMapView(){return mapView;}
    MapScene* getMapScene(){return mapScene;}
    MapDescription* getMapDescription(){return mapdescr;}
    TileReader* getTileReader(){return tilereader;}
    int getLevel();
    void mapGoTo(const GeoPoint &center, int z = -1);
    void getMapPosition(double &lat, double &lon);
    void mapGoToTopLeftPosition(const GeoPoint &gp, int z = -1);
    void getMapTopLeftPosition(double &lat, double &lon);
    QRectF getDisplayedMapRect();
public slots:
    void gotoLayer(int z);
    QString mapProperty();
private slots:
    void doMapGoTo();
signals:
protected:
    DataWdg* datawdg;
    MapDescription* mapdescr;
    Projection* projection;
    TileReader* tilereader;
    MapView* mapView;
    LevelWdg* levelwdg;
    ScaleWdg* scalewdg;
    MapScene* mapScene;
    void resizeEvent(QResizeEvent *event);
};

#endif // MAPWIDGET_H
