#include "topntfrom.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QStandardItemModel>
#include <QSettings>
#include <QApplication>
#include "gpx.h"
#include "util.h"

ToPntFrom::ToPntFrom(QWidget *parent) : QWidget{parent}
{

    setWindowTitle("Before and after point");
    QVBoxLayout* vbl = new QVBoxLayout(this);
    vbl->setContentsMargins(0,0,0,0);
    lbTotal = new QLabel(this);
    tableView = new QTableView(this);
    vbl->addWidget(tableView);
    vbl->addWidget(lbTotal);
    setLayout(vbl);
    model = new QStandardItemModel(6, 3, this);
    model->setHorizontalHeaderItem(0, new QStandardItem(tr("Name")));
    model->setHorizontalHeaderItem(1, new QStandardItem(tr("From the start")));
    model->setHorizontalHeaderItem(2, new QStandardItem(tr("To the finish")));
    tableView->setModel(model);
    //        tableView->verticalHeader()->setDefaultSectionSize(fontMetrics().height()+4);;
    readSettings();
}

void ToPntFrom::closeEvent(QCloseEvent *event)
{
    writeSettings();
    QWidget::closeEvent(event);
}

void ToPntFrom::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("ToPntFrom");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

void ToPntFrom::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
    settings.beginGroup("ToPntFrom");
    if (settings.contains("geometry"))
        restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}

void ToPntFrom::calcCharact(Gpx* gpx, int idx)
{
    double timemove1, distmove1, timestop1, distmovestop1;
    gpx->calcCommonCharact(timemove1, distmove1, timestop1, distmovestop1, 0, idx);
    double timemove2, distmove2, timestop2, distmovestop2;
    gpx->calcCommonCharact(timemove2, distmove2, timestop2, distmovestop2, idx, gpx->tptCount() - 1);
    double time1 = timemove1 + timestop1;
    double time2 = timemove2 + timestop2;
    double dist1 = distmove1 + distmovestop1;
    double dist2 = distmove2 + distmovestop2;
    setRowTable(0, tr("Distance, km"), QString("%1").arg(dist1 * 0.001,0,'f',2), QString("%1").arg(dist2 * 0.001,0,'f',2));
    setRowTable(1, tr("Time, h:m:s"), secToHMS(qRound64(time1)), secToHMS(qRound64(time2)));
    setRowTable(2, tr("Average speed, km/h"), QString("%1").arg(dist1 * 0.001 / (time1 / 3600),0,'f',1),
                QString("%1").arg(dist2 * 0.001 / (time2 / 3600),0,'f',1));
    setRowTable(3, tr("Average speed in motion, km/h"), QString("%1").arg(distmove1 * 0.001 / (timemove1 / 3600),0,'f',1),
                QString("%1").arg(distmove2 * 0.001 / (timemove2 / 3600),0,'f',1));
    setRowTable(4, tr("Time on the move, h:m:s"), secToHMS(qRound64(timemove1)), secToHMS(qRound64(timemove2)));
    setRowTable(5, tr("Time without movement, h:m:s"), secToHMS(qRound64(timestop1)), secToHMS(qRound64(timestop2)));
    lbTotal->setText(tr("Total %1 km, %2 h:m:s, %3 km/h").arg((dist1 + dist2) * 0.001,0,'f',2)
                     .arg(secToHMS(qRound64(time1+time2)))
                     .arg((dist1 + dist2) * 0.001 / ((time1 + time2) / 3600),0,'f',1));
    tableView->resizeColumnsToContents();
}

void ToPntFrom::setRowTable(int row, const QString& charact, const QString& val1, const QString& val2)
{
    model->setItem(row, 0, new QStandardItem(charact));
    model->setItem(row, 1, new QStandardItem(val1));
    model->setItem(row, 2, new QStandardItem(val2));
}
