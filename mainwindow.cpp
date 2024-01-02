#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QToolBar>
#include <QSignalMapper>
#include <QFileDialog>
#include <QInputDialog>
#include <QProgressDialog>
#include <QMessageBox>
#include <QSettings>
#include <QCloseEvent>
#include <QMdiSubWindow>

#include "def.h"
#include "ltime.h"
#include "gcrecalcs.h"
#include "projection.h"
#include "gpx.h"
#include "gpxreader.h"
#include "wptmrk.h"
#include "datawdg.h"
#include "settingsdlg.h"
#include "movetodlg.h"
#include "mapwidget.h"
#include "mapview.h"
#include "mapscene.h"
#include "mapdescription.h"
#include "tilereader.h"
#include "mapinfodlg.h"
#include "topntfrom.h"
#include "reportwdg.h"
#include "weaterdlg.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    xdotpersm = metric(QPaintDevice::PdmPhysicalDpiX)* 0.3937008;
    local_time_offset_index = -1;
    gcrecalcs = new GCRecalcs();
    projections[Projection::MERC_WGS84] = new MercatorPro(tr("WGS-84"), gcrecalcs->getEllipsoid(GCRecalcs::E_WGS84));
    projections[Projection::MERC_S6371000] = new MercatorPro(tr("Sph R=6371000"), gcrecalcs->getEllipsoid(GCRecalcs::E_S6371000));
    projections[Projection::MERC_S6378137] = new MercatorPro(tr("Sph R=6378137"), gcrecalcs->getEllipsoid(GCRecalcs::E_S6378137));
    projections[Projection::GK_KRASOVSKY] = new GKPro(tr("Krasovsky"), gcrecalcs->getEllipsoid(GCRecalcs::E_KRASOVSKY));
    setCentralWidget(ui->mdiArea);


    wptmark = new WptMrk();
    datawdg = new DataWdg(&gpxlist);
    datawdg->setWptMrk(wptmark);

    gpxreader = new GpxReader(this);
    gpxreader->setWptMrk(wptmark);

    ui->dockWidget->setWindowTitle("Gpx files");
    ui->dockWidget->setWidget(datawdg);
    QToolBar* maintoolbar = addToolBar("mainToolBar");
    maintoolbar->setObjectName("mainToolBar");
    maintoolbar->addAction(ui->actionOpenMap);
    maintoolbar->addSeparator();
    maintoolbar->addAction(ui->actionCreateGpx);
    maintoolbar->addAction(ui->actionOpenGpx);
    maintoolbar->addAction(ui->actionSaveGpx);
    maintoolbar->addAction(ui->actionSaveGpxAs);
    maintoolbar->addSeparator();

    QToolBar* trackedtoolbar = addToolBar("trackEdToolBar");
    trackedtoolbar->setObjectName("trackEdToolBar");
    trackedtoolbar->addAction(ui->actionTrackProp);
    trackedtoolbar->addAction(ui->actionMergeTracks);
    trackedtoolbar->addAction(ui->actionCutTrack);
    trackedtoolbar->addAction(ui->actionRemoveEqPoints);
    trackedtoolbar->addAction(ui->actionRemoveAzPoints);

    QToolBar* movingtoolbar = addToolBar("movingToolBar");
    movingtoolbar->setObjectName("movingToolBar");
    movingtoolbar->addAction(ui->actionMoveGpxUp);
    movingtoolbar->addAction(ui->actionMoveGpxDown);
    movingtoolbar->addSeparator();
    movingtoolbar->addAction(ui->actionFirstTrackPoint);
    movingtoolbar->addAction(ui->actionPreviosTrackPoint);
    movingtoolbar->addAction(ui->actionNextTrackPoint);
    movingtoolbar->addAction(ui->actionLastTrackPoint);
    movingtoolbar->addAction(ui->actionMoveByNTreckPoints);
    movingtoolbar->addSeparator();
    movingtoolbar->addAction(ui->actionToNextStop);
    movingtoolbar->addAction(ui->actionToPointWithAltMax);
    movingtoolbar->addAction(ui->actionToPointWithAltMin);
    movingtoolbar->addAction(ui->actionToPointWithSpeedMax);
    movingtoolbar->addSeparator();
    movingtoolbar->addAction(ui->actionMoveToPoint);
    movingtoolbar->addAction(ui->actionMoveToCurrentTrackPoint);
    movingtoolbar->addAction(ui->actionMoveToCurrentWaypoint);
    movingtoolbar->addSeparator();
    movingtoolbar->addAction(ui->actionToPrevWaypoint);
    movingtoolbar->addAction(ui->actionToNextWaypoint);
    movingtoolbar->addAction(ui->actionPrevWaypoint);
    movingtoolbar->addAction(ui->actionNextWaypoint);

    QToolBar* meastoolbar = addToolBar("measToolbar");
    meastoolbar->setObjectName("measToolbar");
    meastoolbar->addAction(ui->actionMeasure);
    meastoolbar->addAction(ui->actionClearMeasure);

    QToolBar* tasktoolbar = addToolBar("taskToolbar");
    tasktoolbar->setObjectName("taskToolbar");
    tasktoolbar->addAction(ui->actionMapProperty);
    tasktoolbar->addAction(ui->actionMovementIndicators);
    tasktoolbar->addAction(ui->actionTripReport);
    tasktoolbar->addAction(ui->actionTownWeather);

    QToolBar* tpedtoolbar = addToolBar("trackPointEdToolbar");
    tpedtoolbar->setObjectName("trackPointEdToolbar");
    tpedtoolbar->addAction(ui->actionAddOnClick);
    tpedtoolbar->addAction(ui->actionInsertOnClick);
    tpedtoolbar->addAction(ui->actionMoveTrackPoint);
    tpedtoolbar->addAction(ui->actionEditTrackPoint);
    tpedtoolbar->addAction(ui->actionDeleteTrackPoints);
    tpedtoolbar->addAction(ui->actionDeletePointsAbove);
    tpedtoolbar->addAction(ui->actionDeletePointsBelow);
    tpedtoolbar->addAction(ui->actionAverageCoordinates);
    tpedtoolbar->addAction(ui->actionCalculateTime);
    tpedtoolbar->addAction(ui->actionEquateCoordToPrec);
    tpedtoolbar->addAction(ui->actionEquateCoordToNext);

    QToolBar* wpedtoolbar = addToolBar("wayPointEdToolbar");
    wpedtoolbar->setObjectName("wayPointEdToolbar");
    wpedtoolbar->addAction(ui->actionAddWayPoint);
    wpedtoolbar->addAction(ui->actionEditWayPoint);
    wpedtoolbar->addAction(ui->actionSortWayPoints);
    wpedtoolbar->addAction(ui->actionAlignWptTpt);
    wpedtoolbar->addAction(ui->actionArrTime);
    wpedtoolbar->addAction(ui->actionDepTime);
    wpedtoolbar->addSeparator();
    wpedtoolbar->addAction(ui->actionDeleteWaypoints);

    QToolBar* viewtoolbar = addToolBar("viewToolbar");
    viewtoolbar->setObjectName("viewToolbar");
    viewtoolbar->addAction(ui->actionTileBoundaries);
    viewtoolbar->addAction(ui->actionHideTrackColumns);
    viewtoolbar->addAction(ui->actionHideWptColumns);

    connect(ui->actionQuit, SIGNAL(triggered()), SLOT(close()));

    connect(ui->actionSettings, SIGNAL(triggered()), SLOT(settings()));
    connect(ui->actionOpenMap, SIGNAL(triggered()), SLOT(openMap()));
    connect(ui->actionCreateGpx, SIGNAL(triggered()), SLOT(createGpx()));
    connect(ui->actionOpenGpx, SIGNAL(triggered()), SLOT(openGpx()));
    connect(ui->actionSaveGpx, SIGNAL(triggered()), SLOT(saveGpx()));
    connect(ui->actionSaveGpxAs, SIGNAL(triggered()), SLOT(saveGpxAs()));
    connect(ui->actionCloseGpx, SIGNAL(triggered()), SLOT(closeGpx()));

    connect(ui->actionMergeTracks, SIGNAL(triggered()), SLOT(mergeGpxs()));
    connect(ui->actionCutTrack, SIGNAL(triggered()), datawdg, SLOT(catGpx()));
    connect(ui->actionRemoveEqPoints, SIGNAL(triggered()), datawdg, SLOT(removeEqPoints()));
    connect(ui->actionRemoveAzPoints, SIGNAL(triggered()), datawdg, SLOT(removeAzPoints()));

    connect(ui->actionDeleteTrackPoints, SIGNAL(triggered()), datawdg, SLOT(deleteTrackPoints()));
    connect(ui->actionDeletePointsAbove, SIGNAL(triggered()), datawdg, SLOT(deletePointsAbove()));
    connect(ui->actionDeletePointsBelow, SIGNAL(triggered()), datawdg, SLOT(deletePointsBelow()));
    connect(ui->actionAverageCoordinates, SIGNAL(triggered()), datawdg, SLOT(averagingTrackCoord()));
    connect(ui->actionEquateCoordToPrec, SIGNAL(triggered()), datawdg, SLOT(equateTrackCoordToPrec()));
    connect(ui->actionEquateCoordToNext, SIGNAL(triggered()), datawdg, SLOT(equateTrackCoordToAfter()));
    connect(ui->actionCalculateTime, SIGNAL(triggered()), datawdg, SLOT(calcTimeOnInterval()));

    connect(ui->actionAddOnClick, SIGNAL(triggered(bool)), SLOT(appendTrackPointOnClick(bool)));
    connect(ui->actionInsertOnClick, SIGNAL(triggered(bool)), SLOT(insertTrackPointOnClick(bool)));
    connect(ui->actionMoveTrackPoint, SIGNAL(triggered(bool)), SLOT(dragTrackPoint(bool)));

    connect(ui->actionPrevWaypoint, SIGNAL(triggered()), datawdg, SLOT(prevWayPoint()));
    connect(ui->actionNextWaypoint, SIGNAL(triggered()), datawdg, SLOT(nextWayPoint()));
    connect(ui->actionToPrevWaypoint, SIGNAL(triggered()), datawdg, SLOT(toPrevWayPoint()));
    connect(ui->actionToNextWaypoint, SIGNAL(triggered()), datawdg, SLOT(toNextWayPoint()));
    connect(ui->actionAddWayPoint, SIGNAL(triggered()), datawdg, SLOT(appendWayPoint()));
    connect(ui->actionEditTrackPoint, SIGNAL(triggered()), datawdg, SLOT(editTrackPoint()));
    connect(ui->actionEditWayPoint, SIGNAL(triggered()), datawdg, SLOT(editWayPoint()));
    connect(ui->actionDeleteWaypoints, SIGNAL(triggered()), datawdg, SLOT(deleteWayPoints()));
    connect(ui->actionAlignWptTpt, SIGNAL(triggered()), datawdg, SLOT(alignWptTpt()));
    connect(ui->actionSortWayPoints, SIGNAL(triggered()), datawdg, SLOT(sortWayPoints()));
    connect(ui->actionArrTime, SIGNAL(triggered()), datawdg, SLOT(setArrivalTime()));
    connect(ui->actionDepTime, SIGNAL(triggered()), datawdg, SLOT(setDepartureTime()));

    connect(ui->actionTileBoundaries, SIGNAL(triggered(bool)), SLOT(tileBorderVisibility(bool)));
    connect(ui->actionHideTrackColumns, SIGNAL(triggered()), datawdg, SLOT(hideTrkColumns()));
    connect(ui->actionHideWptColumns, SIGNAL(triggered()), datawdg, SLOT(hideWptColumns()));
    connect(ui->actionDefaultGpxColWidth, SIGNAL(triggered()), datawdg, SLOT(defaultGpxColumnWidth()));
    connect(ui->actionDefaultTrkColWidth, SIGNAL(triggered()), datawdg, SLOT(defaultTrkColumnWidth()));
    connect(ui->actionDefaultWptColWidth, SIGNAL(triggered()), datawdg, SLOT(defaultWptColumnWidth()));
    connect(ui->actionLocalTimeGpx, SIGNAL(triggered()), SLOT(setLocalTimeGpx()));

    ui->actionDeg->setData(GeoPoint::Deg);
    ui->actionDegMin->setData(GeoPoint::DegMin);
    ui->actionDegMinSec->setData(GeoPoint::DegMinSec);
    ui->actionPixels->setData(GeoPoint::Pixel);
    ui->actionMeters->setData(GeoPoint::Metre);
    ui->actionRad->setData(GeoPoint::Rad);
    ui->actionPixels->setChecked(true);
    GeoPoint::display_format = GeoPoint::Pixel;
    connect(ui->menuCoordinate_system, SIGNAL(triggered(QAction*)), SLOT(setSysCoord(QAction*)));

    connect(ui->actionMoveGpxUp, SIGNAL(triggered()), datawdg, SLOT(moveGpxUp()));
    connect(ui->actionMoveGpxDown, SIGNAL(triggered()), datawdg, SLOT(moveGpxDown()));
    connect(ui->actionFirstTrackPoint, SIGNAL(triggered()), datawdg, SLOT(firstTrackPoint()));
    connect(ui->actionPreviosTrackPoint, SIGNAL(triggered()), datawdg, SLOT(prevTrackPoint()));
    connect(ui->actionNextTrackPoint, SIGNAL(triggered()), datawdg, SLOT(nextTrackPoint()));
    connect(ui->actionLastTrackPoint, SIGNAL(triggered()), datawdg, SLOT(lastTrackPoint()));
    connect(ui->actionMoveByNTreckPoints, SIGNAL(triggered()), datawdg, SLOT(goNPoints()));
    connect(ui->actionToPointWithSpeedMax, SIGNAL(triggered()), datawdg, SLOT(findSpeedMax()));
    connect(ui->actionToPointWithAltMax, SIGNAL(triggered()), datawdg, SLOT(findAltMax()));
    connect(ui->actionToPointWithAltMin, SIGNAL(triggered()), datawdg, SLOT(findAltMin()));
    connect(ui->actionToNextStop, SIGNAL(triggered()), datawdg, SLOT(findNextStop()));

    connect(ui->actionMoveToPoint, SIGNAL(triggered()), SLOT(mapGoToPoint()));
    connect(ui->actionMoveToCurrentTrackPoint, SIGNAL(triggered()), SLOT(mapGoToTrackPoint()));
    connect(ui->actionMoveToCurrentWaypoint, SIGNAL(triggered()), SLOT(mapGoToWayPoint()));

    connect(ui->actionMovementIndicators, SIGNAL(triggered()), SLOT(movementIndicators()));
    connect(ui->actionTrackProp, SIGNAL(triggered()), datawdg, SLOT(trackProperty()));
    connect(ui->actionMapProperty, SIGNAL(triggered()), SLOT(mapProperty()));
    connect(ui->actionTripReport, SIGNAL(triggered()), SLOT(tripReport()));
    connect(ui->actionTownWeather, SIGNAL(triggered()), SLOT(townWeather()));
    connect(ui->actionMeasure, SIGNAL(triggered(bool)), SLOT(measure(bool)));
    connect(ui->actionClearMeasure, SIGNAL(triggered()), SLOT(clearmeasure()));

    connect(ui->actionCloseMap, SIGNAL(triggered()), ui->mdiArea, SLOT(closeActiveSubWindow()));
    connect(ui->actionCloseAllMap, SIGNAL(triggered()), ui->mdiArea, SLOT(closeAllSubWindows()));
    connect(ui->actionTileMap, SIGNAL(triggered()), ui->mdiArea, SLOT(tileSubWindows()));
    connect(ui->actionCascadeMap, SIGNAL(triggered()),ui->mdiArea, SLOT(cascadeSubWindows()));
    connect(ui->actionNextMap, SIGNAL(triggered()), ui->mdiArea, SLOT(activateNextSubWindow()));
    connect(ui->actionPrevMap, SIGNAL(triggered()), ui->mdiArea, SLOT(activatePreviousSubWindow()));
    connect(ui->actionTabbedMap, SIGNAL(triggered(bool)), SLOT(setTabbedView(bool)));

    connect(ui->actionAbout, SIGNAL(triggered()), SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), SLOT(aboutQt()));
    connect(ui->actionTest, SIGNAL(triggered()), SLOT(test()));


    copyright = "<p><span style=\"font-size:10pt; color:#000055;\">Copyright &copy; 2020 Yri Popov</span></p>";
    ui->statusbar->addWidget(ui->lbCoord);
    ui->statusbar->addWidget(ui->lbScale);

//    windowMapper = new QSignalMapper(this);
//    connect(windowMapper, SIGNAL(mappedObject(QObject*)), ui->mdiArea, SLOT(setActiveSubWindow(QMdiSubWindow*)));

    readSettings();
    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), SLOT(subWindowActivated(QMdiSubWindow*)));
    setEnbleAction();
}

MainWindow::~MainWindow()
{
    delete ui;
    if (gcrecalcs)
        delete gcrecalcs;
    delete wptmark;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    bool isCloseApp = checkSave();
    if (isCloseApp)
    {
        writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

bool MainWindow::checkSave()
{
    return true;
}

void MainWindow::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowstate", saveState());
    settings.setValue("coord_display_format", GeoPoint::display_format);
    settings.endGroup();

    settings.beginGroup("Files");
    settings.setValue("gpxdir", Def::gpxdir);
    settings.endGroup();

    writeMapsSettings(settings);

    settings.beginGroup("Gpx");
    settings.setValue("local_time_offset_index", local_time_offset_index);
    settings.setValue("filenames", datawdg->getGpxFileNames());
    settings.endGroup();
    datawdg->writeSettings(settings);
}

void MainWindow::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    bool mainwingeom = settings.contains("geometry");
    if (mainwingeom)
    {
        restoreGeometry(settings.value("geometry").toByteArray());
        restoreState(settings.value("windowstate").toByteArray());
    }
   else
        setGeometry(0,20,800, 600);
    GeoPoint::display_format = static_cast<GeoPoint::DispFormat>(settings.value("coord_display_format",
                                                                                GeoPoint::display_format).toInt());
    settings.endGroup();

    settings.beginGroup("Files");
    Def::gpxdir = settings.value("gpxdir").toString();
    settings.endGroup();

    readMapsSettings(settings);

    settings.beginGroup("Gpx");
    local_time_offset_index = settings.value("local_time_offset_index", -1).toInt();
    QString gpx_filenames = settings.value("filenames").toString();
    if (!gpx_filenames.isEmpty())
        openGpxList(gpx_filenames.split(QChar(';')));
    settings.endGroup();
    datawdg->readSettings(settings);
}

void MainWindow::writeMapsSettings(QSettings& settings)
{
    settings.beginGroup("MDIArea");
    settings.value("mdigeometry", ui->mdiArea->saveGeometry());
    settings.endGroup();
    settings.beginGroup("MapWidgets");
    settings.beginWriteArray("Maps");
    QList<QMdiSubWindow*> sub_win_list = ui->mdiArea->subWindowList();
    for (int i=0; i<sub_win_list.count(); i++)
    {
        settings.setArrayIndex(i);
        MapWidget* mw = dynamic_cast<MapWidget*>(sub_win_list.at(i)->widget());
        if (!mw->getMapScene())
            continue;
        settings.setValue("name", mw->getMapDescription()->getName());
        settings.setValue("projection", mw->getMapDescription()->getProjection());
        settings.setValue("ellipsoid", mw->getMapDescription()->getEllipsoid());
        settings.setValue("z", mw->getMapScene()->getZ());
        double lat, lon;
        mw->getMapPosition(lat, lon);
        settings.setValue("map_position_lat", lat);
        settings.setValue("map_position_lon", lon);
        settings.setValue("width", mw->getMapView()->width());
        settings.setValue("height", mw->getMapView()->height());
        settings.setValue("tile_source", mw->getTileReader()->getCacheloadcontrol());
    }
    settings.endArray();
    settings.endGroup();
}

void MainWindow::readMapsSettings(QSettings& settings)
{
    settings.beginGroup("MDIArea");
    ui->mdiArea->restoreGeometry(settings.value("mdigeometry").toByteArray());
    settings.endGroup();
    settings.beginGroup("MapWidgets");
    int size = settings.beginReadArray("Maps");
    for (int i=0; i<size; i++)
    {
        settings.setArrayIndex(i);
        QString name = settings.value("name").toString();
        MapDescription* mapdescr = nullptr;
        if (name == "OSM")
            mapdescr = new OSMDescription(); //проверить уничтожение
        else if (name == "OTM")
            mapdescr = new OTMDescription();
        else if (name == "Топо")
            mapdescr = nullptr;
        mapdescr->setProjection(settings.value("projection").toString());
        mapdescr->setEllipsoid(settings.value("ellipsoid").toString());
        mapdescr->setZ(settings.value("z").toInt());
        mapdescr->setLat(settings.value("map_position_lat").toDouble());
        mapdescr->setLon(settings.value("map_position_lon").toDouble());
        mapdescr->setWidth(settings.value("width").toInt());
        mapdescr->setHeight(settings.value("height").toInt());
        mapdescr->setTileSource(settings.value("tile_source").toInt());
        createMapWidget(mapdescr);
    }
    settings.endArray();
    settings.endGroup();
    ui->mdiArea->tileSubWindows();
}

void MainWindow::settings()
{
    SettingsDlg* dlg = new SettingsDlg();
    dlg->setWptMark(wptmark);
    dlg->exec();
    delete dlg;
}

void MainWindow::openMap()
{
    QString typemap = QInputDialog::getItem(this, "", "Карта", QStringList() << "OSM" << "OTM" << "Топо");
    if (typemap == "OSM")
    {
        createMapWidget(new OSMDescription());
    }
    else if (typemap == "OTM")
    {
        createMapWidget(new OTMDescription());
    }
    else if (typemap == "Топо")
        QMessageBox::information(this,"","Not implemented!");
}

void MainWindow::openGpx()
{
    QStringList flst = QFileDialog::getOpenFileNames(this,tr("Open"), Def::gpxdir, tr("Files GPX (*.gpx)"));
    openGpxList(flst);
}

void MainWindow::createGpx()
{
    Gpx* gpx = new Gpx();
    gpx->setFileName("noname.gpx");
    datawdg->appendGpx(gpx);
}

void MainWindow::openGpxList(const QStringList& fnames)
{
    if(fnames.count()>0)
    {
        QProgressDialog* progress = new QProgressDialog(tr("Reading..."), tr("Cancel"), 0, fnames.count(), this);
        progress->setMinimumDuration(0);
        progress->setValue(0);
        for(int i=0; i<fnames.count(); i++)
        {
            openGpxFile(fnames.at(i));
            progress->setValue(i);
        }
        delete progress;
    }
}

void MainWindow::openGpxFile(const QString& fname)
{
    QFile file(fname);
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        Gpx* gpx = new Gpx();
        gpx->setFileName(fname);
        gpxreader->readGpx(&file, gpx);
        QDateTime gpx_time = QDateTime::currentDateTime();
        if(gpx->tptCount()>0)
            gpx_time = gpx->trackPointAt(0)->time;
        gpx->setLocalTimeOffset(LTime::local_time_offset(gpx_time, local_time_offset_index));
        if(gpx->noBonds()) gpx->calcBounds();
        for(int i=0; i<gpx->wptCount(); i++)
            wptmark->setImageToWayPoint(gpx->wayPointAt(i));
        datawdg->appendGpx(gpx);
    }
}
void MainWindow::saveGpx()
{
    Gpx* gpx = datawdg->getCurrentGpx();
    if(gpx!=0)
        saveGpx(gpx);
    else
        QMessageBox::information(this, 0, tr("No gpx selected."), QMessageBox::Ok);
}

void MainWindow::saveGpxAs()
{
    Gpx* gpx = datawdg->getCurrentGpx();
    if(gpx!=0)
    {
        QFileInfo fi(gpx->getFileName());
        QString dirname = fi.absolutePath();
        if(dirname.isEmpty())
            dirname = Def::gpxdir;
        else
            dirname = QString("%1/%2").arg(fi.absolutePath()).arg(fi.completeBaseName());
        QString fname = QFileDialog::getSaveFileName(this, tr("Save as..."), dirname, "GPX (*.gpx)");
        if(!fname.isEmpty())
        {
            gpx->setFileName(fname);
            saveGpx(gpx);
        }
    }
}

void MainWindow::saveGpx(Gpx* gpx)
{
    QString fname = gpx->getFileName();
    if (fname.compare("noname.gpx")==0)
        saveGpxAs();
    else
    {
        QFile file(fname);
        if (file.open(QFile::WriteOnly | QFile::Text))
        {
            gpxreader->writeGPX(&file, gpx);
        }
    }
}

void MainWindow::closeGpx()
{
    QList<Gpx*> glst = datawdg->closeGpx();
//    qDebug() << "glst.count" << glst.count();
    while (!glst.isEmpty())
    {
        Gpx* gpx = glst.last();
        if(gpx->isModified())
        {
            int ret = QMessageBox::question(this, "", tr("%1 changed. Save?").arg(gpx->getFileName()),
                                            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if (ret == QMessageBox::Yes)
                saveGpx(gpx);
        }
        glst.removeLast();
    }
}

MapWidget* MainWindow::createMapWidget(MapDescription* mapdescr)
{
    MapWidget* mapwdg = new MapWidget(datawdg, mapdescr, &projections);
    connect(mapwdg->getMapView(), SIGNAL(sendCoordinates(double, double, double)), SLOT(displayCoordinates(double, double, double)));
    connect(mapwdg->getMapScene(), SIGNAL(sendMapsGoToPoint(double,double)), SLOT(mapsGoToPoint(double,double)));
    ui->mdiArea->addSubWindow( mapwdg);
    mapwdg->setVisible(true);

    return mapwdg;  // возврат используется в readMapsSettings
}

void MainWindow::displayCoordinates(const double &x, const double &y, const double &scale)
{
    if (ui->actionPixels->isChecked())
        ui->lbCoord->setText(QString("X: %1  Y: %2").arg(x).arg(y));
    else if (ui->actionDegMinSec->isChecked())
        ui->lbCoord->setText(tr("B: %1  L: %2").arg(GeoPoint::latToDmsStr(x)).arg(GeoPoint::lonToDmsStr(y)));
    else if (ui->actionDegMin->isChecked())
        ui->lbCoord->setText(tr("B: %1  L: %2").arg(GeoPoint::latToDmStr(x)).arg(GeoPoint::lonToDmStr(y)));
    else if (ui->actionDeg->isChecked())
        ui->lbCoord->setText(tr("B: %1  L: %2").arg(GeoPoint::latToDegStr(x)).arg(GeoPoint::lonToDegStr(y)));
    else if (ui->actionMeters->isChecked())
        ui->lbCoord->setText(QString("X: %1  Y: %2").arg(x,0,'f',2).arg(y,0,'f',2));
    else if (ui->actionRad->isChecked())
        ui->lbCoord->setText(tr("B: %1  L: %2").arg(GeoPoint::latToRadStr(x)).arg(GeoPoint::lonToRadStr(y)));
    ui->lbScale->setText(tr("Scale:  %1 м/пиксел;  %2 м/см;   1 : %3").arg(scale,0,'f',2)
                         .arg(scale*xdotpersm,0,'f',2).arg(scale * xdotpersm * 100));
}

QVariant MainWindow::getUnitCoord()
{
    QVariant uc = ui->actionPixels->data();
    if (ui->actionDeg->isChecked())
        uc = ui->actionDeg->data();
    else if (ui->actionDegMin->isChecked())
        uc = ui->actionDegMin->data();
    else if (ui->actionDegMinSec->isChecked())
        uc = ui->actionDegMinSec->data();
    else if (ui->actionMeters->isChecked())
        uc = ui->actionMeters->data();
    else if (ui->actionRad->isChecked())
        uc = ui->actionRad->data();
    return uc;
}

void MainWindow::setSysCoord(QAction* action)
{
    if (ui->actionPixels != action)
        ui->actionPixels->setChecked(false);
    if (ui->actionDeg != action)
        ui->actionDeg->setChecked(false);
    if (ui->actionDegMin != action)
        ui->actionDegMin->setChecked(false);
    if (ui->actionDegMinSec != action)
        ui->actionDegMinSec->setChecked(false);
    if (ui->actionMeters != action)
        ui->actionMeters->setChecked(false);
    if (ui->actionRad != action)
        ui->actionRad->setChecked(false);
    //GeoPoint::display_format - статическая переменная используется во всех объектах приложения
    GeoPoint::display_format = static_cast<GeoPoint::DispFormat>(action->data().toInt());
}

void MainWindow::mapsGoToPoint(const double &lat, const double &lon)
{
    QList<QMdiSubWindow *>	wlist = ui->mdiArea->subWindowList();
    foreach( QMdiSubWindow* mdisw, wlist)
    {
        MapWidget* mw =  qobject_cast<MapWidget*>(mdisw->widget());
        if(mw!=0)
        {
             mw->mapGoTo(GeoPoint(lat, lon));
        }
    }
}

void MainWindow::mergeGpxs()
{
    QString fname = QFileDialog::getSaveFileName(this, tr("Save the merged track"), Def::gpxdir, "GPX (*.gpx)");
    if (!fname.isEmpty())
    {
        if (!QFileInfo::exists(fname))
        {
            Gpx* gpx = new Gpx();
            gpx->setFileName(fname);
            datawdg->mergeGpx(gpx);
            saveGpx(gpx);
        }
        else
            QMessageBox::warning(this, "", tr("A file with this name already exists."), QMessageBox::Ok);
    }
}

void MainWindow::mapGoToPoint()
{
    MapWidget* mw = activeMdiWidget();
    if (mw)
    {
        GeoPoint::DispFormat fmt = static_cast<GeoPoint::DispFormat>(getUnitCoord().toInt());
        double lat, lon;
        mw->getMapPosition(lat, lon);
        int z = mw->getLevel();
        MoveToDlg* dlg = new MoveToDlg();
        GeoPoint gp(lat, lon);
        dlg->setData(gp, z, fmt);
        if (dlg->exec() == QDialog::Accepted)
        {
            dlg->getData(gp, z);
            mw->mapGoTo(gp, z);
        }
        delete dlg;
    }
}

void MainWindow::mapGoToTrackPoint()
{
    TrackPoint* p = datawdg->getCurrentTrackPoint();
    if (p)
    {
        MapWidget* mw = activeMdiWidget();
        int z = mw->getLevel();
        mw->mapGoTo(*p, z);
    }
}

void MainWindow::mapGoToWayPoint()
{
    WayPoint* p = datawdg->getCurrentWayPoint();
    if (p)
    {
        MapWidget* mw = activeMdiWidget();
        int z = mw->getLevel();
        mw->mapGoTo(*p, z);
        int i = datawdg->findNearestPointIndex(p->time);
        datawdg->setCurrentTrackPoint(i);
    }
}

void MainWindow::movementIndicators()
{
    Gpx* gpx = datawdg->getCurrentGpx();
    int trackpointrow = datawdg->getCurrentTrackPointRow();
    if (gpx && trackpointrow > 0)
    {
        ToPntFrom* iwdg = new ToPntFrom();
        iwdg->setAttribute(Qt::WA_DeleteOnClose);
        iwdg->calcCharact(gpx, trackpointrow);
        iwdg->show();
    }
}

void MainWindow::tileBorderVisibility(bool visibility)
{
    MapWidget* mw = activeMdiWidget();
    if (mw)
        mw->getMapScene()->setViewTileBorders(visibility);
}

void MainWindow::setLocalTimeGpx()
{
    QStringList items;
    items << tr("Moscow") << tr("Yekaterinburg");
    bool ok;
    QString item = QInputDialog::getItem(this, "", tr("Timezone:"), items, 0, false, &ok);
    if (ok && !item.isEmpty())
    {
        for(int i=0; i<items.count(); i++)
        {
            if (item == items.at(i))
            {
                local_time_offset_index = i;
                break;
            }
        }
        for(int i=0; i<gpxlist.count(); i++)
        {
            Gpx* gpx = gpxlist.at(i);
            QDateTime gpx_time = QDateTime::currentDateTime();
            if(gpx->tptCount()>0)
                gpx_time = gpx->trackPointAt(0)->time;
            gpxlist.at(i)->setLocalTimeOffset(LTime::local_time_offset(gpx_time, local_time_offset_index));
        }
    }
}

void MainWindow::subWindowActivated(QMdiSubWindow* msw)
{
    QMdiSubWindow* mdisw = dynamic_cast<QMdiSubWindow*>(msw);
    if (mdisw)
    {
        MapWidget* mw = dynamic_cast<MapWidget*>(mdisw);
        if (mw)
            ui->actionTileBoundaries->setChecked(mw->getMapScene()->getViewTileBorders());
    }
}

MapWidget* MainWindow::activeMdiWidget()
{
    QMdiSubWindow* activeSubWindow = ui->mdiArea->activeSubWindow();
    MapWidget *mw = dynamic_cast<MapWidget*>(activeSubWindow->widget());
    return mw;
}

void MainWindow::setTabbedView(bool val)
{
        if (val)
            ui->mdiArea->setViewMode(QMdiArea::TabbedView);
        else
            ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
}

void MainWindow::mapProperty()
{
    MapWidget* mw = activeMdiWidget();
    if (mw)
    {
        MapInfoDlg* mi = new MapInfoDlg();
        mi->setMapProperty(mw->mapProperty());
        mi->exec();
        delete mi;
    }
}

void MainWindow::tripReport()
{
    Gpx* gpx = datawdg->getCurrentGpx();
    if (gpx)
    {
        int wpt_count = gpx->wptCount();
        if (wpt_count > 1)
        {
            ReportWdg* report = new ReportWdg();
            report->setAttribute(Qt::WA_DeleteOnClose);
            report->createReport(gpx);
            report->show();
        }
        else
            QMessageBox::warning(this, "", "There are no marks in the track. Unable to create trip report.");
    }
    else
        QMessageBox::warning(this, "", "No GPX selected.");
}

void MainWindow::townWeather()
{
    WeaterDlg* dlg = new WeaterDlg();
    dlg->exec();
    delete dlg;
}

void MainWindow::appendTrackPointOnClick(bool turn_on)
{
    ui->actionInsertOnClick->setChecked(false);
    ui->actionMoveTrackPoint->setChecked(false);
    MapWidget* mw = activeMdiWidget();
    if (mw)
        mw->getMapView()->appendTrackPointOnClick(turn_on);
}

void MainWindow::insertTrackPointOnClick(bool turn_on)
{
    ui->actionAddOnClick->setChecked(false);
    ui->actionMoveTrackPoint->setChecked(false);
    MapWidget* mw = activeMdiWidget();
    if (mw)
        mw->getMapView()->insertTrackPointOnClick(turn_on);
}

void MainWindow::dragTrackPoint(bool turn_on)
{
    ui->actionAddOnClick->setChecked(false);
    ui->actionInsertOnClick->setChecked(false);
    MapWidget* mw = activeMdiWidget();
    if (mw)
        mw->getMapView()->dragTrackPoint(turn_on);
}

void MainWindow::measure(bool turn_on)
{
    MapWidget* mw = activeMdiWidget();
    if (mw)
        mw->getMapView()->measure(turn_on);
}

void MainWindow::clearmeasure()
{
    datawdg->getMeasure()->deleteAllPoints();
    MapWidget* mw = activeMdiWidget();
    if (mw)
        mw->getMapView()->mapSceneUpdate();
}

void MainWindow::setEnbleAction()
{

}

void MainWindow::updateMenus()
{

}

void MainWindow::about()
{
    QFileInfo fi(QApplication::applicationFilePath());
    QDateTime dt = fi.lastModified();
    QMessageBox::about(this, tr("О программе"),
        tr("<h2 align=\"center\">%1</h2><p align=\"center\">Версия %2</p>"
//           "<div style=\"font-size:128px\">Сборка: %3</div>"
           "<p align=\"center\">Сборка: %3</p>"
           "<p align=\"center\">Copyright &copy; 2013-%4 %5</p>"
           "<p align=\"center\">Сайт программы  <a href=\"http://www.triponmap.ucoz.ru\">http://www.triponmap.ucoz.ru</a></p>"
           "<p align=\"justify\">Эта программа является свободным программным обеспечением. "
           "Вы можете распространять ее и/или модифицировать соблюдая условия "
           "Открытого Лицензионного Соглашения (GNU General Public License) версии 2, "
           "опубликованного Free Software Foundation, или (на ваш выбор) любой последующей версии."
           "<br>Эта программа распространяется в надежде, что она будет полезной, "
           "но БЕЗ ЛЮБОЙ ГАРАНТИИ или ПРИГОДНОСТИ ДЛЯ КОНКРЕТНОЙ ЦЕЛИ."
           "<br>Для разработки программы использовались IDE Qt Creator с компилятором MinGW С++ и "
           "библиотека Qt 6  <a href=\"https://www.qt.io/\">https://qt.io</a>;"
           "<br>Для конвертирования треков в программе используется консольная утилита GPSBabel <a href=\"http://www.gpsbabel.org\">http://www.gpsbabel.org</a>.</p>"
           "<p align=\"justify\"><font color=\"#005000\">Вы можете поддержать дальнейшую разработку программы, сделав пожертвование "
           "через платежную систему Яндекс.Деньги. Номер счета </font><font color=\"#500000\">410011981232738.</font></p>"
           )
           .arg(QApplication::applicationName())
           .arg(QApplication::applicationVersion())
           .arg(dt.toString("dd.MM.yyyy hh:mm"))
           .arg(dt.date().year())
           .arg(QApplication::organizationName()));
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}

//#include "ltime.h"
#include "weatherreader.h"
void MainWindow::test()
{
    WeatherReader w; // = WeatherReader(this);
    w.getWeather(52.5, 42.3333);
    QMessageBox::information(this,"", "Подождите!");
//    qDebug() << w.getResult();
}
