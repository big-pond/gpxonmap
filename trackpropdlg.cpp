#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QComboBox>
#include <QTableWidget>
#include <QDialogButtonBox>
#include <QHeaderView>

#include "trackpropdlg.h"
#include "util.h"
#include "gpx.h"
#include "colorrect.h"

#include <QDebug>

TrackPropDlg::TrackPropDlg(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(4,4,4,4);
    QGridLayout *gl0 = new QGridLayout;
    gl0->setVerticalSpacing(2);
    gl0->addWidget(new QLabel(tr("Файл")),0,0);
    lbFileName = new QLabel();
    gl0->addWidget(lbFileName,0,1);
    gl0->addWidget(new QLabel(tr("Название")), 1, 0);
    leName = new QLineEdit();
    gl0->addWidget(leName,1,1);
    layout->addLayout(gl0);

    QGridLayout *gl2 = new QGridLayout;
    gl2->setVerticalSpacing(2);
    int z = 0;
    gl2->addWidget(new QLabel(tr("Local time")),z,0);
    sbTimeOffset = new QSpinBox();
    sbTimeOffset->setRange(-11, 11);
    sbTimeOffset->setPrefix("UTC  ");
    connect(sbTimeOffset, SIGNAL(valueChanged(int)), SLOT(changePrefix(int)));
//    connect(sbTimeOffset, &QSpinBox::valueChanged, &TrackPropDlg::changePrefix);
    gl2->addWidget(sbTimeOffset, z, 1);
    z++;
    gl2->addWidget(new QLabel(tr("Line width")),z,0);
    sbLineWidth = new QSpinBox();
    sbLineWidth->setMinimum(0);sbLineWidth->setMaximum(20);sbLineWidth->setMaximumWidth(60);
    gl2->addWidget(sbLineWidth,z,1);
    cbxLineVisible = new QCheckBox(tr("Draw line"));
    gl2->addWidget(cbxLineVisible,z,2);
//    gl2->setAlignment(cbxLineVisible,Qt::AlignRight);
    z++;
    gl2->addWidget(new QLabel(tr("Point size")),z,0);
    sbPointSize = new QSpinBox();
    sbPointSize->setMinimum(1); sbPointSize->setMaximum(20);sbPointSize->setMaximumWidth(60);
    gl2->addWidget(sbPointSize,z,1);
    cbxPointVisible = new QCheckBox(tr("Draw points"));
    gl2->addWidget(cbxPointVisible,z,2);
//    gl2->setAlignment(cbxPointVisible,Qt::AlignRight);
    z++;
    gl2->addWidget(new QLabel(tr("Color")),z,0);
    color = new  ColorRect(80, 18); color->setMaximumWidth(80); color->setMinimumHeight(18);
    gl2->addWidget(color,z,1);
    cbxSpeedVisible = new QCheckBox(tr("Show speed by colour")) ;
    gl2->addWidget(cbxSpeedVisible,z,2);
//    gl2->setAlignment(cbxSpeedVisible,Qt::AlignRight);
    gl2->setHorizontalSpacing(20);
    layout->addLayout(gl2);

    lbTabTit1 = new QLabel(tr("Main parameters"));
    layout->addWidget(lbTabTit1);
    layout->setAlignment(lbTabTit1, Qt::AlignCenter);
    twInfo = new QTableWidget(3,4);
    QStringList colh;
    colh << tr("Name") << tr("Time") << tr("Way, km") << tr("Speed,\nkm/h");
    twInfo->setHorizontalHeaderLabels(colh);
    layout->addWidget(twInfo);

    lbTabTit2 = new QLabel(tr("Advanced parameters"));
    layout->addWidget(lbTabTit2);
    layout->setAlignment(lbTabTit2, Qt::AlignCenter);
    twInfo1 = new QTableWidget(3,2);
    QStringList colh1;
    colh1 << tr("Parameter") << tr("Value");
    twInfo1->setHorizontalHeaderLabels(colh1);
    layout->addWidget(twInfo1);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    setLayout(layout);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(cbxLineVisible, SIGNAL(clicked(bool)), SLOT(updateTrackBounds(bool)));
    setWindowTitle(tr("Track property"));
}

void TrackPropDlg::setGpx(Gpx* t)
{
    gpx = t;
    leName->setText(gpx->getName());
    lbFileName->setText(gpx->getFileName());
    sbTimeOffset->setValue(gpx->getLocalTimeOffset());
    color->setColor(gpx->getColor());
    cbxLineVisible->setChecked(gpx->getTrackLineVisible());
    sbLineWidth->setValue(gpx->getLineWidth());
    cbxPointVisible->setChecked(gpx->getTrackPointVisible());
    sbPointSize->setValue(gpx->getPointSize());
    cbxSpeedVisible->setChecked(gpx->getDrawSpeed());

    calcTrackProperty();
}

void TrackPropDlg::changePrefix(int val)
{
    if (val == 0)
        sbTimeOffset->setPrefix("UTC  ");
    else if (val > 0)
        sbTimeOffset->setPrefix("UTC +");
    else
        sbTimeOffset->setPrefix("UTC -");
}

void TrackPropDlg::calcTrackProperty()
{
    double timemove, timestop;
    gpx->calcMoveStopTime(timemove, timestop);
//    qDebug() << timemove << timestop;
    double time = timemove + timestop;
    double dist = gpx->getLength();
    int row = 0;
    addRowToTabl(twInfo, row, tr("All track"), dist, qRound(time));
    row++;
    addRowToTabl(twInfo, row, tr("Motion"), dist, qRound(timemove));
    row++;
    addRowToTabl(twInfo, row, tr("Standing"), 0, qRound(timestop));
    for(int i=0; i<twInfo->rowCount(); i++)
        for(int j=1; j<twInfo->columnCount(); j++)
            twInfo->item(i, j)->setTextAlignment(Qt::AlignRight);

    int idx = 0;
    row = 0;
    twInfo1->setItem(row,0, new QTableWidgetItem(tr("Maximum speed, km/h")));
    twInfo1->setItem(row,1, new QTableWidgetItem(QString::number(gpx->getSpeedMax(idx),'f',1)));
    row++;
    double hmax = gpx->getAltitudeMax(idx);
    double hmin = gpx->getAltitudeMin(idx);
    twInfo1->setItem(row,0, new QTableWidgetItem(tr("Maximum altitude, m")));
    twInfo1->setItem(row,1, new QTableWidgetItem(QString::number(hmax,'f',1)));
    row++;
    twInfo1->setItem(row,0, new QTableWidgetItem(tr("Minimum altitude, m")));
    twInfo1->setItem(row,1, new QTableWidgetItem(QString::number(hmin,'f',1)));
    for(int i=0; i<twInfo1->rowCount(); i++)
        for(int j=1; j<twInfo1->columnCount(); j++)
            twInfo1->item(i, j)->setTextAlignment(Qt::AlignRight);

    twInfo->resizeColumnsToContents();
    twInfo->resizeRowsToContents();
    twInfo1->resizeColumnsToContents();
    twInfo1->resizeRowsToContents();

    twInfo->setMaximumHeight((twInfo->rowCount()+2)*(twInfo->horizontalHeader()->geometry().height()+1));
    twInfo1->setMaximumHeight((twInfo1->rowCount()+1)*(twInfo1->horizontalHeader()->geometry().height()+1));
}

void TrackPropDlg::addRowToTabl(QTableWidget* tw, const int& row, const QString& nm, const double& dist, const qint64& time)
{
    QChar z('0');
    int h, m, s;
    QString sdist = "-", ssped = sdist;
    if(dist>0)
    {
        sdist = QString::number(dist*0.001,'f',3);
        ssped = QString::number(dist/time*3.6,'f',1);
    }
    secToHMS(time, h, m, s);
    tw->setItem(row,0, new QTableWidgetItem(nm));
    tw->setItem(row,1, new QTableWidgetItem(QString("%1:%2:%3").arg(h).arg(m,2,10,z).arg(s,2,10,z)));
    tw->setItem(row,2, new QTableWidgetItem(sdist));
    tw->setItem(row,3, new QTableWidgetItem(ssped));
}

/*virtual*/
void TrackPropDlg::accept ()
{
    QDialog::accept();
    if(gpx->getName()!=leName->text())
    {
        gpx->setName(leName->text());
    }
    if(color->isChangedColor())
    {
        gpx->setColor(color->getColor().name());
        gpx->setModified(true);
    }
    if(cbxLineVisible->isChecked()!=gpx->getTrackLineVisible())
        gpx->setTrackLineVisible(cbxLineVisible->isChecked());
    if(sbLineWidth->value()!=gpx->getLineWidth())
    {
        gpx->setLineWidth(sbLineWidth->value());
        gpx->setModified(true);
    }
    if(cbxPointVisible->isChecked()!=gpx->getTrackPointVisible())
        gpx->setTrackPointVisible(cbxPointVisible->isChecked());
    if(sbPointSize->value()!=gpx->getPointSize())
        gpx->setPointSize(sbPointSize->value());
    if(cbxSpeedVisible->isChecked()!=gpx->getDrawSpeed())
    {
        gpx->setDrawSpeed(cbxSpeedVisible->isChecked());
    }
}

void TrackPropDlg::updateTrackBounds(bool val)
{
    if(val)
    {
        double latmax = gpx->getMaxLat();
        double latmin = gpx->getMinLat();
        double lonmax = gpx->getMaxLon();
        double lonmin = gpx->getMinLon();
        if(latmax<=latmin || lonmax<=lonmin)
            gpx->calcBounds();
    }
}
