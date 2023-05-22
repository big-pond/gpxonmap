#include <QGridLayout>
#include <QLabel>
#include <QDateTimeEdit>
#include <QDialogButtonBox>

#include "pointeditdlg.h"

#include "gpointwdg.h"


PointEditDlg::PointEditDlg(QWidget *parent) : QDialog(parent)
{
    gpointwdg = new GPointWdg(this);
    dteTime = new QDateTimeEdit();
    dteTime->setMaximumWidth(140);
    dteTime->setDisplayFormat("dd.MM.yyyy H:mm:ss");
    dteTime->setTimeSpec(Qt::UTC);
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    gl = new QGridLayout();
    gl->addWidget(new QLabel(tr("Время UTC")),0,0);
    gl->addWidget(dteTime,0,1);
    gl->addWidget(new QWidget(),0,2);
    mainlayout = new QVBoxLayout();
    mainlayout->addWidget(gpointwdg);
    mainlayout->addLayout(gl);
    mainlayout->addWidget(buttonBox);
    setLayout(mainlayout);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

PointEditDlg::~PointEditDlg()
{

}

void PointEditDlg::setTrackPoint(TrackPoint* val, int cf)
{
    p = val;
    gpointwdg->setGeoCoordinate(*p, static_cast<GeoPoint::DispFormat>(cf));
    dteTime->setDateTime(p->time);
}

void PointEditDlg::accept()
{
    QDialog::accept();

    GeoPoint gc = gpointwdg->getGeoCoordinate();
    if(p->latitude != gc.latitude) p->latitude = gc.latitude;
    if(p->longitude != gc.longitude) p->longitude = gc.longitude;
    if(p->altitude != gc.altitude) p->altitude = gc.altitude;
    if(p->time!=dteTime->dateTime()) p->time = dteTime->dateTime();
}
