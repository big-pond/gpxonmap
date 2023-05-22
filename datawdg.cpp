#include "datawdg.h"
#include "ui_datawdg.h"
#include <algorithm>

#include <QSettings>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QInputDialog>
#include <QProgressDialog>
//#include <QModelIndexList>

#include "gpx.h"
#include "gpxmodel.h"
#include "trackmodel.h"
#include "wptmodel.h"
#include "trackpropdlg.h"
#include "pointeditdlg.h"
#include "wpteditdlg.h"
#include "hidcoldlg.h"
#include <QDebug>

int DataWdg::go_step = 100;
bool DataWdg::remove_in_model = false;

DataWdg::DataWdg(QList<Gpx *> *gpxlst, QWidget *parent) : QWidget(parent), ui(new Ui::DataWdg)
{
    ui->setupUi(this);
    gpxlist = gpxlst;
    measure = new Gpx(this);
    gpxmodel = new GpxModel(this);
    gpxmodel->setGpxList(gpxlist);
    ui->tvGpx->setModel(gpxmodel);
    ui->tvGpx->verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 8);
    trackmodel = new TrackModel(this);
    ui->tvTrack->setModel(trackmodel);
    ui->tvTrack->verticalHeader()->setDefaultSectionSize(fontMetrics().height()+8);
    wptmodel = new WptModel(this);
    ui->tvWayPoints->setModel(wptmodel);
    ui->tvWayPoints->verticalHeader()->setDefaultSectionSize(fontMetrics().height()+8);
    connect(ui->tvGpx->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
            SLOT(gpxCurrentRowChanged(QModelIndex, QModelIndex)));

    defaultGpxColumnWidth();
    defaultTrkColumnWidth();
    defaultWptColumnWidth();
}

DataWdg::~DataWdg()
{
    delete ui;
}

void DataWdg::writeSettings(QSettings& settings)
{
    settings.beginGroup("DataWdg");
    settings.setValue("step", go_step);
    settings.setValue("splitter", ui->splitter->saveState());
    settings.setValue("splitter_2", ui->splitter_2->saveState());
    settings.setValue("remove_in_model", remove_in_model);

    settings.beginGroup("gpxcolumns");
    settings.beginWriteArray("colcount");
    for(int i=0; i<GpxModel::COLCOUNT; i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("width", ui->tvGpx->columnWidth(i));
    }
    settings.endArray();
    settings.endGroup();

    settings.beginGroup("trackcolumns");
    settings.beginWriteArray("colcount");
    for(int i=0; i<TrackModel::COLCOUNT; i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("width", ui->tvTrack->columnWidth(i));
    }
    settings.endArray();
    settings.endGroup();

    settings.beginGroup("wptcolumns");
    settings.beginWriteArray("colcount");
    for(int i=0; i<WptModel::COLCOUNT; i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("width", ui->tvWayPoints->columnWidth(i));
    }
    settings.endArray();
    settings.endGroup();

    settings.endGroup(); //DataWdg
}

void DataWdg::readSettings(QSettings& settings)
{
    settings.beginGroup("DataWdg");
    go_step = settings.value("gostep", go_step).toInt();
    ui->splitter->restoreState(settings.value("splitter").toByteArray());
    ui->splitter_2->restoreState(settings.value("splitter_2").toByteArray());
    remove_in_model = settings.value("remove_in_model", remove_in_model).toBool();

    int w = fontMetrics().boundingRect("0").width();
    settings.beginGroup("gpxcolumns");
    int size = settings.beginReadArray("colcount");
    for(int i=0; i<size; i++)
    {
        settings.setArrayIndex(i);
        int width = settings.value("width", w * GpxModel::defaultColW[i]).toInt();
        if (width > 0)
            ui->tvGpx->setColumnWidth(i, width);
        else
            ui->tvGpx->setColumnHidden(i, true);
    }
    settings.endArray();
    settings.endGroup();

    settings.beginGroup("trackcolumns");
    size = settings.beginReadArray("colcount");
    for(int i=0; i<size; i++)
    {
        settings.setArrayIndex(i);
        int width = settings.value("width", w * TrackModel::defaultColW[i]).toInt();
        if (width > 0)
            ui->tvTrack->setColumnWidth(i, width);
        else
            ui->tvTrack->setColumnHidden(i, true);
    }
    settings.endArray();
    settings.endGroup();

    settings.beginGroup("wptcolumns");
    size = settings.beginReadArray("colcount");
    for(int i=0; i<size; i++)
    {
        settings.setArrayIndex(i);
        int width = settings.value("width", w * WptModel::defaultColW[i]).toInt();
        if (width > 0)
            ui->tvWayPoints->setColumnWidth(i, width);
        else
            ui->tvWayPoints->setColumnHidden(i, true);
    }
    settings.endArray();
    settings.endGroup();

    settings.endGroup(); //DataWdg
}

void DataWdg::defaultGpxColumnWidth()
{
    int w = fontMetrics().boundingRect("0").width();
    for(int col=0; col<GpxModel::COLCOUNT; col++)
        ui->tvGpx->setColumnWidth(col, w * GpxModel::defaultColW[col]);
}

void DataWdg::defaultTrkColumnWidth()
{
    int w = fontMetrics().boundingRect("0").width();
    for(int col=0; col<TrackModel::COLCOUNT; col++)
        ui->tvTrack->setColumnWidth(col, w * TrackModel::defaultColW[col]);
}

void DataWdg::defaultWptColumnWidth()
{
    int w = fontMetrics().boundingRect("0").width();
    for(int col=0; col<WptModel::COLCOUNT; col++)
        ui->tvWayPoints->setColumnWidth(col, w * WptModel::defaultColW[col]);
}

void DataWdg::appendGpx(Gpx *gpx)
{
    gpxmodel->appendGpx(gpx);
    ui->tvGpx->setCurrentIndex(gpxmodel->index(gpxmodel->rowCount() - 1, 1));
}

QTableView *DataWdg::getTrackTableView()
{
    return ui->tvTrack;
}

Gpx* DataWdg::getCurrentGpx()
{
    Gpx* gpx = 0;
    QModelIndex index = ui->tvGpx->currentIndex();
    if (index.isValid())
        gpx = (*gpxlist)[index.row()];
    return gpx;
}

int DataWdg::getCurrentTrackPointRow()
{
    int row = -1;
    QModelIndex index = ui->tvTrack->currentIndex();
    if (index.isValid())
        row = index.row();
    return row;
}

TrackPoint* DataWdg::getCurrentTrackPoint()
{
    TrackPoint* p = 0;
    QModelIndex index = ui->tvTrack->currentIndex();
    if (index.isValid())
    {
        Gpx* gpx = getCurrentGpx();
        p = gpx->trackPointAt(index.row());
    }
    return p;
}

WayPoint* DataWdg::getCurrentWayPoint()
{
    WayPoint* p = 0;
    QModelIndex index = ui->tvWayPoints->currentIndex();
    if (index.isValid())
    {
        Gpx* gpx = getCurrentGpx();
        p = gpx->wayPointAt(index.row());
    }
    return p;
}

QString DataWdg::getGpxFileNames()
{
    QString s;
    for(int i=0; i<gpxlist->count(); i++)
        s += QString("%1;").arg(gpxlist->at(i)->getFileName());
    if (s.length()>0)
        s.chop(1);
    return s;
}

void DataWdg::gpxCurrentRowChanged(const QModelIndex& current, const QModelIndex& /*previous*/)
{
    Gpx empty_gpx;
    if (current.isValid())
    {
        int row = current.row();
        trackmodel->setGpx(gpxlist->at(row));
        ui->tvTrack->selectRow(0);
        wptmodel->setGpx(gpxlist->at(row));
        ui->tvWayPoints->selectRow(0);
    }
    else
    {
        trackmodel->setGpx(&empty_gpx);
        wptmodel->setGpx(&empty_gpx);
    }
}

void  DataWdg::trackProperty()
{
    QModelIndex index = ui->tvGpx->currentIndex();
    qDebug() << index;
    if (index.isValid())
    {
        TrackPropDlg* dlg = new TrackPropDlg();
        Gpx* cur_gpx = gpxlist->at(index.row());
        dlg->setGpx(cur_gpx);
        if (dlg->exec() == QDialog::Accepted)
            ;
        delete dlg;
    }
    else
        QMessageBox::information(this, "", tr("Track is not selected."), QMessageBox::Ok);
}

bool lessThan(const QModelIndex &i1, const QModelIndex &i2)
{
    return i1.row() < i2.row();
}

QList<Gpx*> DataWdg::closeGpx()
{
    QList<Gpx*> glst; //Для передачи в MainWindow для сохранения (если измерялись) и удаления
    QModelIndexList ilst = ui->tvGpx->selectionModel()->selectedRows();
    //    for (int i=0; i<ilst.count(); i++) qDebug() << ilst.at(i);
    if (ilst.count() > 0)
    {
        // Без следующей строки вылетает при закрытии единственного трека, если строка точек трека была выделена
        ui->tvTrack->setCurrentIndex(ui->tvTrack->currentIndex().sibling(-1, -1));
        std::sort(ilst.begin(), ilst.end(), lessThan);
        //        for (int i=0; i<ilst.count(); i++) qDebug() << ilst.at(i);
        int sr = ilst[0].row();
        while(!ilst.isEmpty())
        {
            int row = ilst.last().row();
            glst << gpxlist->at(row);
            gpxmodel->removeRow(row);
            ilst.removeLast();
        }
        ui->tvGpx->selectRow(sr);
    }
    return glst;
}

void  DataWdg::mergeGpx(Gpx* gpx)
{
    QModelIndexList ilst = ui->tvGpx->selectionModel()->selectedRows();
    if (ilst.count() > 0)
    {
        int i = 0;
        QProgressDialog* progress = new QProgressDialog(tr("Join..."), tr("Cancel"), 0, ilst.count(), this);
        progress->setWindowModality(Qt::WindowModal);
        progress->setMinimumDuration(0);
        foreach(QModelIndex index, ilst)
        {
            if (index.column() == 0)  // только строки
            {
                gpx->appendGpx(gpxlist->at(index.row()));
                i++;
                progress->setValue(i);
                QApplication::processEvents();
            }
        }
        progress->setValue(ilst.count());
        appendGpx(gpx);
        delete progress;
    }
}

void  DataWdg::catGpx()
{
    QModelIndexList ilst = ui->tvGpx->selectionModel()->selectedRows();
    if (ilst.count() == 1)
    {
        int gpx_row = ilst[0].row();
        Gpx* gpx = gpxlist->at(gpx_row);
        int row = ui->tvTrack->currentIndex().row();
        if (row>0 && row<gpx->tptCount()-1)
        {
            Gpx* gpx1 = gpx->getPartCopy(0, row);
            Gpx* gpx2 = gpx->getPartCopy(row, gpx->tptCount()-1);
            gpxmodel->appendGpx(gpx1);
            gpxmodel->appendGpx(gpx2);
        }
    }
    else
        QMessageBox::information(this, "", tr("Select one gpx."));
}

void DataWdg::removeEqPoints()
{
    //Удаляет точку если азимут на нее менюше 1°, а траектория пройдет от нее на расстоянии менее 1 м
    QModelIndexList ilst = ui->tvGpx->selectionModel()->selectedRows();
    if (ilst.count() == 1)  // Чтобы не было неоднозначности если выбрано несколько
    {
        clock_t start, end;
        start = clock();
        int n = 0;
        if (remove_in_model)
            n = removeEquPoints(); //  17.061 сек
        else
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            n = gpxlist->at(ilst[0].row())->removeEquPoints(); // 0.109 сек
            QApplication:: restoreOverrideCursor();
        }
        end = clock();
        double time = (end - start) / (double)CLOCKS_PER_SEC;
        qDebug() << "Время" << time;

        if (n > 0)
            QMessageBox::information(this, "", tr("%1 points removed.").arg(n), QMessageBox::Ok);
    }
}

int DataWdg::removeEquPoints()
{
    int n = 0;
    QModelIndexList ilst = ui->tvGpx->selectionModel()->selectedRows();
    if (ilst.count() == 1)  // Чтобы небыло неоднозначности если выбрано несколько
    {

        int i=0, js=0, count=0;
        bool calccount = false;
        QProgressDialog* progress = new QProgressDialog(tr("Removing redundant points..."), tr("Stop"), 0,
                                                        trackmodel->rowCount());
        progress->setWindowModality(Qt::WindowModal);
        while (i < trackmodel->rowCount() - 5)
        {
            if (trackmodel->trackPoint(i)->latitude == trackmodel->trackPoint(i+1)->latitude
             && trackmodel->trackPoint(i)->longitude == trackmodel->trackPoint(i+1)->longitude
             && trackmodel->trackPoint(i)->latitude == trackmodel->trackPoint(i+2)->latitude
             && trackmodel->trackPoint(i)->longitude == trackmodel->trackPoint(i+2)->longitude
             && trackmodel->trackPoint(i)->latitude == trackmodel->trackPoint(i+3)->latitude
             && trackmodel->trackPoint(i)->longitude == trackmodel->trackPoint(i+3)->longitude
             && trackmodel->trackPoint(i)->latitude == trackmodel->trackPoint(i+4)->latitude
             && trackmodel->trackPoint(i)->longitude == trackmodel->trackPoint(i+4)->longitude)
            {
                if (!calccount)
                {
                    js = i + 2;
                    calccount = true;
                    count = 0;
                    i++;
                }
                else
                {
                    count++;
                    i++;
                }
            }
            else
            {
                calccount = false;
                i++;
                if (count > 0)
                {
                    trackmodel->removeRows(js, count, trackmodel->index(js, 0).parent());
                    i = js;
                    n += count;
                    count = 0;
                }
            }
            progress->setValue(i + n);
            if (progress->wasCanceled())
                break;
        }
        delete progress;
        if (n > 0)
            gpxlist->at(ilst[0].row())->setModified(true);
    }
    return n;
}

void DataWdg::removeAzPoints()
{
    //Удаляет точку если азимут на нее менюше 1°, а траектория пройдет от нее на расстоянии менее 1 м
    QModelIndexList ilst = ui->tvGpx->selectionModel()->selectedRows();
    if (ilst.count() == 1)  // Чтобы не было неоднозначности если выбрано несколько
    {
        clock_t start, end;
        start = clock();
        int n = 0;
        if (remove_in_model)
            n = removeAzPoints(1.0, 1.0); // 3607.21 сек
        else
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            n = gpxlist->at(ilst[0].row())->removeAzPoints(1.0, 1.0); //34.674 сек
            QApplication:: restoreOverrideCursor();
        }
        end = clock();
        double time = (end - start) / (double)CLOCKS_PER_SEC;
        qDebug() << "Время" << time;

        if (n > 0)
            QMessageBox::information(this, "", tr("%1 points removed.").arg(n), QMessageBox::Ok);
    }
}

int DataWdg::removeAzPoints(const double& da, const double& ds)
{
    // Удаляет точку p если азимут на нее меньше da и линия таектории после удаления p пройдет
    // от нее на расстоянии меньше ds
    int n = 0;
    int i = 0;
    QProgressDialog* progress = new QProgressDialog(tr("Removing redundant points..."), tr("Stop"), 0,
                                                    trackmodel->rowCount());
    progress->setWindowModality(Qt::WindowModal);
    while (i < trackmodel->rowCount() - 2)
    {
        if (trackmodel->trackPoint(i+1)->latitude == trackmodel->trackPoint(i+2)->latitude
                && trackmodel->trackPoint(i+1)->longitude == trackmodel->trackPoint(i+2)->longitude)
            i += 3;
        else
        {
            double az1 = trackmodel->trackPoint(i)->sphAzimuthTo(*trackmodel->trackPoint(i + 1));
            double az2 = trackmodel->trackPoint(i + 1)->sphAzimuthTo(*trackmodel->trackPoint(i + 2));
            double az3 = trackmodel->trackPoint(i)->sphAzimuthTo(*trackmodel->trackPoint(i + 2));
            double d = trackmodel->trackPoint(i)->sphDistanceTo(*trackmodel->trackPoint(i + 1));
            GeoPoint p = trackmodel->trackPoint(i)->atSphDistanceAndAzimuth(d, az3);
            double d_s = trackmodel->trackPoint(i + 1)->sphDistanceTo(p);
            double delta = abs(az1 - az2);
            if (delta < da && d_s < ds)
            {
                trackmodel->removeRow(i + 1);
                n++;
            }
            else
                i += 1;
        }
        progress->setValue(i + n);
        if (progress->wasCanceled())
            break;
    }
    delete progress;
    return n;
}

//void DataWdg::appendTrackPoint(TrackPoint* p)
//{
//    int rows = trackmodel->rowCount();
//    bool success = trackmodel->insertRow(rows);
//    if (success)
//    {
//        TrackPoint* tp = trackmodel->trackPoint(rows);
//        tp->latitude = p->latitude;
//        tp->longitude = p->longitude;
//        tp->altitude = p->altitude;
//        tp->time = p->time;
//    }
//}

bool DataWdg::appendTrackPoint(const double &lat, const double &lon)
{
    int rows = trackmodel->rowCount();
    bool success = trackmodel->insertRow(rows);
    if (success)
    {
        TrackPoint* tp = trackmodel->trackPoint(rows);
        tp->latitude = lat;
        tp->longitude = lon;
        if (rows>0)
        {
            tp->altitude = trackmodel->data(trackmodel->index(rows-1, TrackModel::Alt)).toDouble();
            tp->time = trackmodel->data(trackmodel->index(rows-1, TrackModel::UTCTime)).toDateTime();
        }
        else
        {
            tp->altitude = 0;
            tp->time = QDateTime::currentDateTimeUtc();
        }
    }
    return success;
}

bool DataWdg::insertTrackPoint(const double &lat, const double &lon)
{
    int row = getCurrentTrackPointRow();
    bool success = false;
    if (row>=0)
    {
        success = trackmodel->insertRow(row);
        if (success)
        {
            TrackPoint* tp = trackmodel->trackPoint(row);
            tp->latitude = lat;
            tp->longitude = lon;
            if(row>0)
            {
                trackmodel->calcTimeOnInterval(row - 1, row + 1);
                trackmodel->calcAltOnInterval(row - 1, row + 1);
            }
            else if (row == 0)
            {
                tp->altitude = trackmodel->data(trackmodel->index(row+1, TrackModel::Alt)).toDouble();
                tp->time = trackmodel->data(trackmodel->index(row+1, TrackModel::UTCTime)).toDateTime();
            }
        }
    }
    return success;
}

void DataWdg::deleteTrackPoints()
{
    QModelIndexList ilst = ui->tvTrack->selectionModel()->selectedRows();
    if (ilst.count() > 0)
    {
        if (QMessageBox::question(this, "", tr("Delete selected points?"), QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes) == QMessageBox::Yes)
        {
            std::sort(ilst.begin(), ilst.end(), lessThan);
            int cr = ilst[0].row();
            while (ilst.count() > 0)
            {
                int row = ilst.last().row();
                trackmodel->removeRow(row);
                ilst.removeLast();
            }
            ui->tvTrack->selectRow(cr);
        }
    }
    else
    {
        QModelIndex i = ui->tvTrack->currentIndex();
        if (i.isValid())
        {
            if (QMessageBox::question(this, "", tr("Delete current point?"), QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::Yes) == QMessageBox::Yes)
            {
                int cr = i.row();
                trackmodel->removeRow(cr);
                ui->tvTrack->selectRow(cr);
            }
        }
    }
}

void DataWdg::deletePointsAbove()
{
    QModelIndex index = ui->tvTrack->currentIndex();
    if (index.isValid())
        if (QMessageBox::question(this, "", tr("Delete all points above the selected one?"),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes) == QMessageBox::Yes)
            trackmodel->removeRows(0, index.row());
}

void DataWdg::deletePointsBelow()
{
    QModelIndex index = ui->tvTrack->currentIndex();
    if (index.isValid())
        if (QMessageBox::question(this, "", tr("Delete all points below the selected one?"),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes) == QMessageBox::Yes)
            trackmodel->removeRows(index.row() + 1, trackmodel->rowCount() - index.row() - 1, index.parent());
}

void DataWdg::averagingTrackCoord()
{
    QModelIndexList ilst = ui->tvTrack->selectionModel()->selectedRows();
    if (ilst.count() > 0)
        if (QMessageBox::question(this, "", tr("Аverage coordinates of selected points?"),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes) == QMessageBox::Yes)
        {
            std::sort(ilst.begin(), ilst.end(), lessThan);
            Gpx* cur_gpx = gpxlist->at(ui->tvGpx->currentIndex().row());
            cur_gpx->averagingTrackCoord(ilst[0].row(), ilst[ilst.count()-1].row());
        }
}

void DataWdg::equateTrackCoordToPrec()
{
    QModelIndexList ilst = ui->tvTrack->selectionModel()->selectedRows();
    if (ilst.count() > 0)
        if (QMessageBox::question(this, "", tr("Аverage coordinates of selected points?"),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes) == QMessageBox::Yes)
        {
            std::sort(ilst.begin(), ilst.end(), lessThan);
            Gpx* cur_gpx = gpxlist->at(ui->tvGpx->currentIndex().row());
            cur_gpx->equateTrackCoordToPrec(ilst[0].row(), ilst[ilst.count()-1].row());
        }
}

void DataWdg::equateTrackCoordToAfter()
{
    QModelIndexList ilst = ui->tvTrack->selectionModel()->selectedRows();
    if (ilst.count() > 0)
        if (QMessageBox::question(this, "", tr("Аverage coordinates of selected points?"),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes) == QMessageBox::Yes)
        {
            std::sort(ilst.begin(), ilst.end(), lessThan);
            Gpx* cur_gpx = gpxlist->at(ui->tvGpx->currentIndex().row());
            cur_gpx->equateTrackCoordToAfter(ilst[0].row(), ilst[ilst.count()-1].row());
        }
}

void DataWdg::calcTimeOnInterval()
{
    QModelIndexList ilst = ui->tvTrack->selectionModel()->selectedRows();
    if (ilst.count() > 0)
        if (QMessageBox::question(this, "", tr("Calculate time at intermediate points with the condition that "
                                               "the speed of movement is constant?"),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
        {
            std::sort(ilst.begin(), ilst.end(), lessThan);
            Gpx* cur_gpx = gpxlist->at(ui->tvGpx->currentIndex().row());
            cur_gpx->calcTimeOnInterval(ilst[0].row(), ilst[ilst.count()-1].row());
        }
}

void DataWdg::moveGpxUp()
{
    QModelIndexList ilst = ui->tvGpx->selectionModel()->selectedRows();
    if (ilst.count() == 1)
    {
        int row = ilst[0].row();
        if (row > 0)
        {
//            gpxlist->swapItemsAt(row-1, row);
            gpxlist->swap(row-1, row);
            ui->tvGpx->selectRow(row-1);
        }
    }
    else
        QMessageBox::information(this, "", "Select one gpx.");
}

void DataWdg::moveGpxDown()
{
    QModelIndexList ilst = ui->tvGpx->selectionModel()->selectedRows();
    if (ilst.count() == 1)
    {
        int row = ilst[0].row();
        if (row < (gpxlist->count()-1))
        {
//            gpxlist->swapItemsAt(row, row+1); Qt 6.2.0
            gpxlist->swap(row, row+1);
            ui->tvGpx->selectRow(row+1);
        }
    }
    else
        QMessageBox::information(this, "", "Choose one row.");
}

void DataWdg::firstTrackPoint()
{
    ui->tvTrack->selectRow(0);
}

void DataWdg::prevTrackPoint()
{
    QModelIndex index = ui->tvTrack->currentIndex();
    if (index.isValid())
    {
        int i = index.row() - 1;
        if (i >= 0)
            ui->tvTrack->selectRow(i);
    }
}

void DataWdg::nextTrackPoint()
{
    QModelIndex index = ui->tvTrack->currentIndex();
    if (index.isValid())
    {
        int i = index.row() + 1;
        if (i < index.model()->rowCount())
            ui->tvTrack->selectRow(i);
    }
}

void DataWdg::lastTrackPoint()
{
    QModelIndex index = ui->tvTrack->currentIndex();
    if (index.isValid())
    {
        int i = index.model()->rowCount() - 1;
        ui->tvTrack->selectRow(i);
    }
}

void DataWdg::goNPoints()
{
    QModelIndex index = ui->tvTrack->currentIndex();
    if (index.isValid())
    {
        bool ok;
        int rowcount = index.model()->rowCount();
        int row = index.row();
        int step = QInputDialog::getInt(this, "", tr("Enter the number of lines(+/-):"),
                                        DataWdg::go_step, -row, rowcount-row-1, 1, &ok);  // Введите количество строк(+/-):
        if (ok)
        {
            DataWdg::go_step = step;
            int i = row + step;
            if (i>=0 && i < rowcount) //Тут исправлено а в datawdg.py ОШИБКА!!!!!!!!!! if 0 <= i < (index.model()->rowCount() - 1 + step):
                ui->tvTrack->selectRow(i);
        }
    }
    else
        QMessageBox::information(this, "", tr("No row selected."), QMessageBox::Ok); // Не выбрана строка.
}

int DataWdg::findNearestPointIndex(const QDateTime &t)
{
    Gpx* cur_gpx = gpxlist->at(ui->tvGpx->currentIndex().row());
    int i = cur_gpx->findPointIndex(t);
    return i;
}

void DataWdg::setCurrentTrackPoint(int i)
{
    if (i>=0 && i<trackmodel->rowCount())
        ui->tvTrack->selectRow(i);
}

void DataWdg::prevWayPoint()
{
    Gpx* gpx = getCurrentGpx();
    if (gpx != 0)
    {
        QModelIndex index = ui->tvWayPoints->currentIndex();
        if (index.isValid())
            if (index.row() > 0)
            {
                int row = index.row() - 1;
                int tpt_idx = gpx->findPointIndex(gpx->wayPointAt(row)->time);
                ui->tvWayPoints->selectRow(row);
                if (tpt_idx > 0)
                    ui->tvTrack->selectRow(tpt_idx);
            }
    }
}

void DataWdg::nextWayPoint()
{
    Gpx* gpx = getCurrentGpx();
    if (gpx != 0)
    {
        QModelIndex index = ui->tvWayPoints->currentIndex();
        if (index.isValid())
            if (index.row() < gpx->wptCount() - 1)
            {
                int row = index.row() + 1;
                int tpt_idx = gpx->findPointIndex(gpx->wayPointAt(row)->time);
                ui->tvWayPoints->selectRow(row);
                if (tpt_idx > 0)
                    ui->tvTrack->selectRow(tpt_idx);
            }
    }

}

void DataWdg::toPrevWayPoint()
{
    Gpx* gpx = getCurrentGpx();
    if (gpx != 0)
    {
        QModelIndex index = ui->tvTrack->currentIndex();
        if (index.isValid())
        {
            int track_row;
            int wpt_row = gpx->prevWayPointIdx(index.row(), track_row);
            ui->tvTrack->selectRow(track_row);
            ui->tvWayPoints->selectRow(wpt_row);
        }
    }
}

void DataWdg::toNextWayPoint()
{
    Gpx* gpx = getCurrentGpx();
    if (gpx)
    {
        QModelIndex index = ui->tvTrack->currentIndex();
        if (index.isValid())
        {
            int track_row;
            int wpt_row = gpx->nextWayPointIdx(index.row(), track_row);
            ui->tvTrack->selectRow(track_row);
            ui->tvWayPoints->selectRow(wpt_row);
        }
    }
}

void DataWdg::findSpeedMax()
{
    QModelIndex index = ui->tvGpx->currentIndex();
    if (index.isValid())
    {
        Gpx* cur_gpx = gpxlist->at(index.row());
        int idx;
        cur_gpx->getSpeedMax(idx);
        ui->tvTrack->selectRow(idx);
    }
}

void DataWdg::findAltMax()
{
    QModelIndex index = ui->tvGpx->currentIndex();
    if (index.isValid())
    {
        Gpx* cur_gpx = gpxlist->at(index.row());
        int idx;
        cur_gpx->getAltitudeMax(idx);
        ui->tvTrack->selectRow(idx);
    }
}

void DataWdg::findAltMin()
{
    QModelIndex index = ui->tvGpx->currentIndex();
    if (index.isValid())
    {
        Gpx* cur_gpx = gpxlist->at(index.row());
        int idx;
        cur_gpx->getAltitudeMin(idx);
        ui->tvTrack->selectRow(idx);
    }
}

void DataWdg::findNextStop()
{
    QModelIndex index0 = ui->tvGpx->currentIndex();
    if (index0.isValid())
    {
        Gpx* cur_gpx = gpxlist->at(index0.row());
        QModelIndex index = ui->tvTrack->currentIndex();
        if (index.isValid())
        {
            int start_idx = index.row(), end_idx;
            /*double sum_sec =*/ cur_gpx->findNextStop(start_idx, end_idx);
            // qDebug() << sum_sec << start_idx << end_idx;
            ui->tvTrack->selectRow(start_idx);
        }
    }
}

void DataWdg::editTrackPoint()
{
    QModelIndex index = ui->tvTrack->currentIndex();
    if (index.isValid())
    {
        PointEditDlg* dlg = new PointEditDlg();
        int r = index.row();
        TrackPoint* p = trackmodel->trackPoint(r);
        dlg->setTrackPoint(p, GeoPoint::display_format);
        dlg->exec();
        delete dlg;
    }
}

void DataWdg::appendWayPoint()  // Добавляет точку в конец, а затем сортирует (нет смысла разделять append и insert
{
    QModelIndex trk_index = ui->tvTrack->currentIndex();
    if (trk_index.isValid())
    {
        WptEditDlg* dlg = new WptEditDlg();
        Gpx* cur_gpx = gpxlist->at(ui->tvGpx->currentIndex().row());
        dlg->setGpx(cur_gpx);
        dlg->setMarking(wptmrk);
        TrackPoint* p = trackmodel->trackPoint(trk_index.row());
        WayPoint wpt(p->latitude, p->longitude, p->altitude, p->time);
        dlg->setWayPoint(&wpt, GeoPoint::display_format);
        if (dlg->exec() == QDialog::Accepted)
        {
            int rows = wptmodel->rowCount();
            qDebug() << rows;
            bool success = wptmodel->insertRow(rows);
            qDebug() << success;
            if (success)
            {
                *(wptmodel->wayPoint(rows)) = wpt;
                cur_gpx->sortWaypoints(); // сортируем по времени
            }
        }
    }
    else if ((wptmodel->rowCount()==0)&&(getCurrentGpx()!=0))
    {
        wptmodel->insertRow(0);
    }
}

void DataWdg::editWayPoint()
{
    QModelIndex index = ui->tvWayPoints->currentIndex();
    if (index.isValid())
    {
        WptEditDlg* dlg = new WptEditDlg();
        Gpx* cur_gpx = gpxlist->at(ui->tvGpx->currentIndex().row());
        dlg->setGpx(cur_gpx);
        dlg->setMarking(wptmrk);
        int r = index.row();
        WayPoint* p = wptmodel->wayPoint(r);
        dlg->setWayPoint(p, GeoPoint::display_format);
        if (dlg->exec() == QDialog::Accepted)
            cur_gpx->sortWaypoints();  //Сортируем по времени, вдруг оно изменилось
        delete dlg;
    }
}

void DataWdg::deleteWayPoints()
{
    QModelIndexList ilst = ui->tvWayPoints->selectionModel()->selectedRows();
    if (ilst.count() > 0)
    {
        if (QMessageBox::question(this, "", tr("Delete selected waypoints?"), QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes) == QMessageBox::Yes)
        {
            std::sort(ilst.begin(), ilst.end(), lessThan);
            int cr = ilst[0].row();
            while (ilst.count() > 0)
            {
                int row = ilst.last().row();
                wptmodel->removeRow(row);
                ilst.removeLast();
            }
            ui->tvWayPoints->selectRow(cr);
        }
    }
    else  // обязательно - работает когда выделена не вся строка, а ячека
    {
        QModelIndex i = ui->tvWayPoints->currentIndex();
        if (i.isValid())
        {
            if (QMessageBox::question(this, "", tr("Delete current waypoint?"), QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::Yes) == QMessageBox::Yes)
            {
                int cr = i.row();
                wptmodel->removeRow(cr);
                ui->tvWayPoints->selectRow(cr);
            }
        }
    }
}

void DataWdg::sortWayPoints()
{
    wptmodel->sortByTime();
}

void DataWdg::alignWptTpt()
{
    QModelIndex wpt_index = ui->tvWayPoints->currentIndex();
    QModelIndex tpt_index = ui->tvTrack->currentIndex();
    if (wpt_index.isValid() && tpt_index.isValid())
    {
        QVariant lat = trackmodel->data(tpt_index.sibling(tpt_index.row(), 0), Qt::EditRole);
        wptmodel->setData(wpt_index.sibling(wpt_index.row(), 1), lat);

        QVariant lon = trackmodel->data(tpt_index.sibling(tpt_index.row(), 1), Qt::EditRole);
        wptmodel->setData(wpt_index.sibling(wpt_index.row(), 2), lon);

        QVariant alt = trackmodel->data(tpt_index.sibling(tpt_index.row(), 2), Qt::EditRole);
        wptmodel->setData(wpt_index.sibling(wpt_index.row(), 3), alt);

        QVariant t = trackmodel->data(tpt_index.sibling(tpt_index.row(), 3), Qt::EditRole);
        wptmodel->setData(wpt_index.sibling(wpt_index.row(), 4), t);
    }
}

void DataWdg::setArrivalTime()
{
    QModelIndex gpx_index = ui->tvGpx->currentIndex();
    QModelIndex wpt_index = ui->tvWayPoints->currentIndex();
    QModelIndex tpt_index = ui->tvTrack->currentIndex();
    if (!gpx_index.isValid() || !wpt_index.isValid() || !tpt_index.isValid())
    {
        QMessageBox::information(this, "", tr("No rows selected."));
        return;
    }
    if (QMessageBox::question(this, "", tr("Set arrival time at waypoint using time of current track point?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
    {
        Gpx* cur_gpx = gpxlist->at(gpx_index.row());
        QDateTime t = cur_gpx->trackPointAt(tpt_index.row())->time;
        cur_gpx->wayPointAt(wpt_index.row())->setArr(t);
    }
}

void DataWdg::setDepartureTime()
{
    QModelIndex gpx_index = ui->tvGpx->currentIndex();
    QModelIndex wpt_index = ui->tvWayPoints->currentIndex();
    QModelIndex tpt_index = ui->tvTrack->currentIndex();
    if (!gpx_index.isValid() || !wpt_index.isValid() || !tpt_index.isValid())
    {
        QMessageBox::information(this, "", tr("No rows selected."));
        return;
    }
    if (QMessageBox::question(this, "", tr("Set departure time at the waypoint using time of current track point?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
    {
        Gpx* cur_gpx = gpxlist->at(gpx_index.row());
        QDateTime t = cur_gpx->trackPointAt(tpt_index.row())->time;
        cur_gpx->wayPointAt(wpt_index.row())->setDep(t);
    }
}

void DataWdg::changePointCoordinates(const double& lat, const double& lon)
{
    int row = getCurrentTrackPointRow();
    TrackPoint* tp = trackmodel->trackPoint(row);
    tp->latitude = lat;
    tp->longitude = lon;
}


 void DataWdg::hideTrkColumns()
 {
    HidColDlg* hcdlg = new HidColDlg();
    hcdlg->setTableView(ui->tvTrack);
    if (hcdlg->exec() == QDialog::Accepted)
    {}
    delete hcdlg;
 }

 void DataWdg::hideWptColumns()
 {
     HidColDlg* hcdlg = new HidColDlg();
     hcdlg->setTableView(ui->tvWayPoints);
     if (hcdlg->exec() == QDialog::Accepted)
     {}
     delete hcdlg;
 }
