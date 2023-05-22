#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>
#include <QMap>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class GCRecalcs;
class Projection;
class Gpx;
class GpxReader;
class WptMrk;
class QSignalMapper;
class DataWdg;
class MapWidget;
class MapDescription;
class QMdiSubWindow;
class QSettings;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void settings();
    void openMap();
    void openGpx();
    void createGpx();
    void saveGpx();
    void saveGpxAs();
    void closeGpx();
    void mergeGpxs();
    void updateMenus();
    void about();
    void aboutQt();
    void test();
    void mapGoToPoint();
    void displayCoordinates(const double& x, const double& y, const double& scale);
    void setSysCoord(QAction *action);
    void mapsGoToPoint(const double& lat, const double& lon);
    void tileBorderVisibility(bool visibility);
    void subWindowActivated(QMdiSubWindow *msw);
    void setTabbedView(bool val);
    void setLocalTimeGpx();
    void mapProperty();
    void mapGoToTrackPoint();
    void mapGoToWayPoint();
    void movementIndicators();
    void tripReport();
    void townWeather();
    void appendTrackPointOnClick(bool turn_on);
    void insertTrackPointOnClick(bool turn_on);
    void dragTrackPoint(bool turn_on);
    void measure(bool turn_on);
    void clearmeasure();
private:
    Ui::MainWindow *ui;
    QSignalMapper *windowMapper;
    double xdotpersm;
    GCRecalcs* gcrecalcs;
    QMap<int, Projection*> projections;
    QList<Gpx*> gpxlist;
    GpxReader* gpxreader;
    WptMrk* wptmark;
    QString copyright;
    DataWdg* datawdg;
    int local_time_offset_index;
    void readSettings();
    void writeSettings();
    bool checkSave();
    void setEnbleAction();
    void openGpxList(const QStringList &fnames);
    void openGpxFile(const QString &fname);
    void saveGpx(Gpx *gpx);
    MapWidget *createMapWidget(MapDescription *mapdescr);
    QVariant getUnitCoord();
    MapWidget* activeMdiWidget();
    void writeMapsSettings(QSettings &settings);
    void readMapsSettings(QSettings &settings);
};
#endif // MAINWINDOW_H
