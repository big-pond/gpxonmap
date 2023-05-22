#ifndef MAPVIEW_H
#define MAPVIEW_H
#include <QGraphicsView>

class DataWdg;
class RubberBand;

class MapView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MapView(DataWdg* dw, QWidget *parent = nullptr);
    ~MapView();
    RubberBand* getRubberband(){return rubberband;}
    QRectF getDisplayedMapRect();
    void appendTrackPointOnClick(bool turn_on);
    void insertTrackPointOnClick(bool turn_on);
    void dragTrackPoint(bool turn_on);
    void measure(bool turn_on);
    void mapSceneUpdate();
public slots:
    void trackCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);
signals:
    void sendMouseMoveXY(double x, double y);
    void sendCoordinates(double x, double y, double scale);
private:
    DataWdg* datawdg;
    RubberBand* rubberband;
    QCursor* measurecursor;
    QCursor* addpcursor;
    QCursor* dragpcursor;
    QImage* measurepoint;
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // MAPVIEW_H
