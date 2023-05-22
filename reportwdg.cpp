#include "reportwdg.h"
#include <QVBoxLayout>
#include <QToolBar>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QSettings>
#include <QApplication>
#include <QMessageBox>
#include <gpx.h>
#include "util.h"

const int ReportWdg::COL_COUNT = 14;
const int ReportWdg::COL_COUNT2 = 3;
const int ReportWdg::defaultColW[] = {25, 12, 18, 18, 10, 12, 10, 12, 10, 10, 10, 10, 12, 10};

ReportWdg::ReportWdg(QWidget *parent)
    : QWidget{parent}
{
    setWindowTitle("Trip report");
    codec_name = "utf-8";
    float_separator = ",";
    QVBoxLayout* mlayout = new QVBoxLayout();
    QToolBar* toolbar = new QToolBar();
    toolbar->addWidget(new QLabel("Codec:"));
    cbCodec = new QComboBox();
    cbCodec->addItems(QStringList() << "UTF-8" << "Windows-1251");
    toolbar->addWidget(cbCodec);
    toolbar->addWidget(new QLabel("  Separator"));
    cbSeparator = new QComboBox();
    cbSeparator->addItems(QStringList() << " " << ",");
    toolbar->addWidget(cbSeparator);
    // acSave = toolbar.addAction(QIcon(":/resources/saveas.png"), r("Save to *.csv file"));
    pbSaveCsv = new QPushButton("Save to CSV file");
    connect(pbSaveCsv, SIGNAL(clicked()), SLOT(saveCsv()));
    toolbar->addWidget(pbSaveCsv);
    toolbar->addSeparator();
    // acCodec = toolbar.addAction(QIcon(":/resources/codec.png"), r("Choose codec to save"));
    lbTimeOffset = new QLabel();
    toolbar->addWidget(lbTimeOffset);
    mlayout->addWidget(toolbar);
    tableView = new QTableView(this);
    tableView->setMinimumSize(80, 200);
    mlayout->addWidget(tableView);
    mlayout->addWidget(new QLabel("<strong>Итоговые значения:</strong>"));
    tableViewSum = new QTableView(this);
    int sum_height = 155;
    tableViewSum->setMaximumHeight(sum_height);
    tableViewSum->setMinimumHeight(sum_height);
    mlayout->addWidget(tableViewSum);
    mlayout->setSpacing(0);
    setLayout(mlayout);
    model = new QStandardItemModel(this);
    model->setColumnCount(ReportWdg::COL_COUNT);
    model->setHorizontalHeaderItem(ReportWdg::RPoint, new QStandardItem(tr("Пункт")));
    model->setHorizontalHeaderItem(ReportWdg::RDist, new QStandardItem(tr("Расстояние")));
    model->setHorizontalHeaderItem(ReportWdg::RArr, new QStandardItem(tr("Прибытие")));
    model->setHorizontalHeaderItem(ReportWdg::RDep, new QStandardItem(tr("Отправление")));
    model->setHorizontalHeaderItem(ReportWdg::RStopt, new QStandardItem(tr("Стоянка")));
    model->setHorizontalHeaderItem(ReportWdg::RStopd, new QStandardItem(tr("Дист. на\nстоянке")));
    model->setHorizontalHeaderItem(ReportWdg::RMovet, new QStandardItem(tr("Время\nдвижения")));
    model->setHorizontalHeaderItem(ReportWdg::RSpeed, new QStandardItem(tr("Скорость")));
    model->setHorizontalHeaderItem(ReportWdg::RTrjamt, new QStandardItem(tr("Время\nпробки")));
    model->setHorizontalHeaderItem(ReportWdg::RTrjamd, new QStandardItem(tr("Дист.\nпробки")));
    model->setHorizontalHeaderItem(ReportWdg::RStopti, new QStandardItem(tr("Время на\nостановках")));
    model->setHorizontalHeaderItem(ReportWdg::RStopdi, new QStandardItem(tr("Дист. на\nостановках")));
    model->setHorizontalHeaderItem(ReportWdg::RPath, new QStandardItem(tr("Путь")));
    model->setHorizontalHeaderItem(ReportWdg::RFuel, new QStandardItem(tr("Заправлено\nостаток")));
    tableView->setModel(model);
    tableView->verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 4);
    sum_model = new QStandardItemModel(this);
    sum_model->setColumnCount(ReportWdg::COL_COUNT2);
    sum_model->setHorizontalHeaderItem(ReportWdg::RParam, new QStandardItem(tr("Parameter")));
    sum_model->setHorizontalHeaderItem(ReportWdg::RValue, new QStandardItem(tr("Value")));
    sum_model->setHorizontalHeaderItem(ReportWdg::RNote2, new QStandardItem(tr("Note")));
    tableViewSum->setModel(sum_model);
    tableViewSum->verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 4);

    readSettings();

}

void ReportWdg::closeEvent(QCloseEvent *event)
{
    writeSettings();
    QWidget::closeEvent(event);
}

void ReportWdg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);

    settings.beginGroup("ReportWdg");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("codec_name", codec_name);
    settings.setValue("float_separator", float_separator);
    settings.endGroup();

    settings.beginGroup("ReportColumns");
    settings.beginWriteArray("colcount");
    for (int i=0; i<model->columnCount(); i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("width", tableView->columnWidth(i));
    }
    settings.endArray();
    settings.endGroup();

    settings.beginGroup("ReportSumColumns");
    settings.beginWriteArray("colcount");
    for (int i=0; i<sum_model->columnCount(); i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("width", tableViewSum->columnWidth(i));
    }
    settings.endArray();
    settings.endGroup();
}

void ReportWdg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
        settings.beginGroup("ReportWdg");
        bool geom = settings.contains("geometry");
        if (geom)
            restoreGeometry(settings.value("geometry").toByteArray());
        else
            setGeometry(50, 50, 500, 300);
        codec_name = settings.value("codec_name", "utf-8").toString();
        float_separator = settings.value("float_separator", ",").toString();
        settings.endGroup();

        int w = fontMetrics().boundingRect("0").width();
        settings.beginGroup("ReportColumns");
        int size = settings.beginReadArray("colcount");
        for (int i=0; i<size; i++)
        {
            settings.setArrayIndex(i);
            int width = settings.value("width", w * ReportWdg::defaultColW[i]).toInt();
            if (width > 0)
                tableView->setColumnWidth(i, width);
            else
                tableView->setColumnHidden(i, true);
        }
        settings.endArray();
        settings.endGroup();

        settings.beginGroup("ReportSumColumns");
        size = settings.beginReadArray("colcount");
        for (int i=0; i<size; i++)
        {
            settings.setArrayIndex(i);
            int width = settings.value("width", w * ReportWdg::defaultColW[i]).toInt();
            if (width > 0)
                tableViewSum->setColumnWidth(i, width);
            else
                tableViewSum->setColumnHidden(i, true);
        }
        settings.endArray();
        settings.endGroup();
}

QList<QStandardItem*>* ReportWdg::createRow(int col_count)
{
    QList<QStandardItem*>* items = new QList<QStandardItem*>();
    for (int col=0; col<col_count; col++)
        items->append(new QStandardItem());
    return items;
}

void ReportWdg::createReport(Gpx *gpx)
{
    int wpt_count = gpx->wptCount();
    int t_offset = gpx->getLocalTimeOffset();
    if (t_offset > 0)
        lbTimeOffset->setText(tr("Time: UTC + %1").arg(t_offset));
    else if (t_offset < 0)
        lbTimeOffset->setText(tr("Time: UTC - %1").arg(abs(t_offset)));
    else  // t_offset == 0
        lbTimeOffset->setText("Time: UTC");
//    QDateTime timestart = gpx->trackPointAt(0)->time;
//    QDateTime timefinish = gpx->trackPointAt(gpx->tptCount()-1)->time;
    double dist_sum = 0.0, move_t_sum = 0.0, stop_t_sum = 0.0, stop_d_sum = 0.0;
    double stop_it_sum = 0.0, stop_id_sum = 0.0;
    double trafjam_t_sum = 0.0, trafjam_d_sum = 0.0;
    model->insertRow(0, *createRow(ReportWdg::COL_COUNT));
    model->setData(model->index(0, ReportWdg::RPoint), gpx->wayPointAt(0)->name()); // Пункт
    model->setData(model->index(0, ReportWdg::RDep), gpx->wayPointAt(0)->dep().addSecs(t_offset*3600));  // Отправление
    double fuel_sum =gpx->wayPointAt(0)->fuel();
    if (fuel_sum > 0)
        model->setData(model->index(0, ReportWdg::RFuel), QString::number(fuel_sum, 'f', 2));
    // add the rest of the lines to the report
    int prevwpt_index = 0;
    while (prevwpt_index < wpt_count - 1)
    {
        int nextwpt_index = gpx->nextControlWayPointIndex(prevwpt_index);
        double dist, move_t, speed, stop_it, stop_id, trafjam_t, trafjam_d;
        gpx->controlIntervalData(prevwpt_index, nextwpt_index, dist, move_t, speed, stop_it, stop_id,
                                 trafjam_t, trafjam_d);
        WayPoint* nwpt = gpx->wayPointAt(nextwpt_index);
        int row = model->rowCount();
        model->insertRow(row, *createRow(ReportWdg::COL_COUNT));
        model->setData(model->index(row, ReportWdg::RPoint), nwpt->name());  // Пункт
        model->setData(model->index(row, ReportWdg::RDist), QString::number(dist * 0.001, 'f', 3));  // Расст.,км
        QDateTime arr = nwpt->arr().addSecs(t_offset*3600);
        model->setData(model->index(row, ReportWdg::RArr), arr);  // Прибытие
        if (nwpt->arr() != nwpt->dep())
        {
            QDateTime dep = nwpt->dep().addSecs(t_offset*3600);
            model->setData(model->index(row, ReportWdg::RDep), dep);  // Отправление
            qint64 sec = nwpt->arr().secsTo(nwpt->dep());
            // find parking distance
            int pi1 = gpx->findPointIndex(nwpt->arr());
            int pi2 = gpx->findPointIndex(nwpt->dep());
            double stop_d = gpx->getDistance(pi1, pi2);
            stop_d_sum += stop_d;
            if (sec > 0)
            {
                stop_t_sum += sec;
                model->setData(model->index(row, ReportWdg::RStopt), secToHMS(sec));
                model->setData(model->index(row, ReportWdg::RStopd), QString::number(stop_d * 0.001, 'f', 3));
            }
        }
        model->setData(model->index(row, ReportWdg::RSpeed), QString::number(speed, 'f', 2));  // Скорость
        if (move_t > 0)  // Время движения
        {
            move_t_sum += move_t;
            model->setData(model->index(row, ReportWdg::RMovet), secToHMS(move_t));
        }
        if (trafjam_t > 0)
        {
            trafjam_t_sum += trafjam_t;
            trafjam_d_sum += trafjam_d;
            model->setData(model->index(row, ReportWdg::RTrjamt), secToHMS(trafjam_t));
            model->setData(model->index(row, ReportWdg::RTrjamd), QString::number(trafjam_d * 0.001, 'f', 3));
        }
        if (stop_it > 0)  // Время (продолжительность) остановок на интервале между контрольными пунктами (WC и т.д.)
        {
            stop_it_sum += stop_it;
            stop_id_sum += stop_id;
            model->setData(model->index(row, ReportWdg::RStopti), secToHMS(stop_it));
            model->setData(model->index(row, ReportWdg::RStopdi), QString::number(stop_id * 0.001, 'f', 3));
        }
        if (nwpt->fuel() > 0)  // Топливо
        {
            if (nextwpt_index < wpt_count - 1)
                fuel_sum += nwpt->fuel();
            else
                fuel_sum -= nwpt->fuel();
        }
            model->setData(model->index(row, ReportWdg::RFuel),  QString::number(nwpt->fuel(), 'f', 2));
        dist_sum += dist;
        model->setData(model->index(row, ReportWdg::RPath),  QString::number(dist_sum * 0.001, 'f', 3));
        prevwpt_index = nextwpt_index;
    }
    int row = model->rowCount();
    model->insertRow(row, *createRow(ReportWdg::COL_COUNT));  // Итого
    model->setData(model->index(row, ReportWdg::RPoint), "И Т О Г О :");
    if (dist_sum > 0)
    {
        model->setData(model->index(row, ReportWdg::RDist), QString::number(dist_sum * 0.001, 'f', 3));
        model->setData(model->index(row, ReportWdg::RPath), QString::number((dist_sum + stop_d_sum)*0.001, 'f', 3));
    }
    if (stop_t_sum > 0)
    {
        model->setData(model->index(row, ReportWdg::RStopt), secToHMS(stop_t_sum));
        model->setData(model->index(row, ReportWdg::RStopd), QString::number(stop_d_sum * 0.001, 'f', 3));
    }
    if (move_t_sum > 0)  // Время движения
    {
        model->setData(model->index(row, ReportWdg::RMovet), secToHMS(move_t_sum));
        double speed = dist_sum / move_t_sum * 3.6;  // km/h
        model->setData(model->index(row, ReportWdg::RSpeed), QString::number(speed, 'f', 2));  // Скорость;
    }
    if (trafjam_t_sum > 0)
    {
        model->setData(model->index(row, ReportWdg::RTrjamt), secToHMS(trafjam_t_sum));
        model->setData(model->index(row, ReportWdg::RTrjamd), QString::number(trafjam_d_sum * 0.001, 'f', 3));
    }
    if (stop_it_sum > 0)  // Время (продолжительность) остановок на интервалах между контрольными пунктами (WC и т.д.)
    {
        model->setData(model->index(row, ReportWdg::RStopti), secToHMS(stop_it_sum));
        model->setData(model->index(row, ReportWdg::RStopdi), QString::number(stop_id_sum * 0.001, 'f', 3));
    }
    if (fuel_sum > 0)  // Топливо
        model->setData(model->index(row, ReportWdg::RFuel), QString::number(fuel_sum, 'f', 2));

    row = sum_model->rowCount();
    sum_model->insertRow(row, *createRow(ReportWdg::COL_COUNT2));
    double dsum = dist_sum + stop_d_sum + stop_id_sum;
    sum_model->setData(sum_model->index(row, ReportWdg::RParam), "Distance, km");  // Расст.,км
    sum_model->setData(sum_model->index(row, ReportWdg::RValue), QString::number(dsum * 0.001, 'f', 3));
    row++;
    sum_model->insertRow(row, *createRow(ReportWdg::COL_COUNT2));
    double t = move_t_sum + stop_t_sum + stop_it_sum;  // + time_in_traffic_jams_sum
    sum_model->setData(sum_model->index(row, ReportWdg::RParam), "Time, h:m:s");
    sum_model->setData(sum_model->index(row, ReportWdg::RValue), secToHMS(t));
    row++;
    sum_model->insertRow(row, *createRow(ReportWdg::COL_COUNT2));
    double speed = 0;
    if (move_t_sum > 0)
        speed = dist_sum / move_t_sum * 3.6;  // km/h;
    sum_model->setData(sum_model->index(row, ReportWdg::RParam), "Average speed of movement, km/h");
    sum_model->setData(sum_model->index(row, ReportWdg::RValue), QString::number(speed, 'f', 2));  // Скорость
    sum_model->setData(sum_model->index(row, ReportWdg::RNote2), "No distance at parking and stops");
    row++;
    sum_model->insertRow(row, *createRow(ReportWdg::COL_COUNT2));
    if (t > 0)
        speed = dsum / t * 3.6;  // km/h
    else
        speed = 0;
    sum_model->setData(sum_model->index(row, ReportWdg::RParam), "Average speed overall, km/h");
    sum_model->setData(sum_model->index(row, ReportWdg::RValue), QString::number(speed, 'f', 2));  // Скорость
    sum_model->setData(sum_model->index(row, ReportWdg::RNote2), "During the whole trip");
    if (dsum>0)
    {
        row++;
        sum_model->insertRow(row, *createRow(ReportWdg::COL_COUNT2));
        sum_model->setData(sum_model->index(row, ReportWdg::RParam),"Car fuel consumption, l/(100 km)");
        sum_model->setData(sum_model->index(row, ReportWdg::RValue), QString::number(fuel_sum/(dsum*0.00001),'f',2));
        sum_model->setData(sum_model->index(row, ReportWdg::RNote2), "");
    }
    // TODO указать дату время начала и время окончания поездки с учетом часового пояса и UTC
}

