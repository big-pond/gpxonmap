#include <QGridLayout>
#include <QLabel>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QToolButton>
#include <QDialogButtonBox>

#include "wpteditdlg.h"

#include "gpointwdg.h"
#include "wptmrk.h"
#include "waypoint.h"
#include "gpx.h"

#include <QDebug>

WptEditDlg::WptEditDlg(QWidget *parent) : QDialog(parent)
{
    gpointwdg = new GPointWdg(this);
    dteTime = new QDateTimeEdit();
    dteTime->setMaximumWidth(140);
    dteTime->setDisplayFormat("dd.MM.yyyy H:mm:ss");
    dteTime->setTimeSpec(Qt::UTC);
    cbMarking = new QComboBox();
    cbMarking->setToolTip(tr("Дополнительная маркировка точки трека"));
    sbFuel = new QDoubleSpinBox();
    sbFuel->setRange(-1000, 1000);
    sbFuel->setValue(0);
    leName = new QLineEdit();
    leCmt = new QLineEdit();
    leDesc = new QLineEdit();
    leSrc = new QLineEdit();
    leSym = new QLineEdit();
    leType = new QLineEdit();
    chControl = new QCheckBox("Контрольная точка");

    dteArr = new QDateTimeEdit();
    dteArr->setMaximumWidth(140);
    dteArr->setDisplayFormat("dd.MM.yyyy H:mm:ss");
    dteArr->setTimeSpec(Qt::UTC);

    dteDep = new QDateTimeEdit();
    dteDep->setMaximumWidth(140);
    dteDep->setDisplayFormat("dd.MM.yyyy H:mm:ss");
    dteDep->setTimeSpec(Qt::UTC);

    tbCalcArr = new QToolButton();
    tbCalcArr->setText("...");
    tbCalcDep = new QToolButton();
    tbCalcDep->setText("...");


    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    int r = 0;
    gl = new QGridLayout();
    gl->addWidget(gpointwdg,r,0,4,3);
    gl->addWidget(new QWidget(),r,3,4,1);
    r += 4;
    gl->addWidget(new QLabel(tr("Время UTC")),r,0);
    gl->addWidget(dteTime,r,1);
    r++;
    gl->addWidget(new QLabel(tr("Маркировка")),r,0);
    gl->addWidget(cbMarking,r,1);
    r++;
    gl->addWidget(new QLabel(tr("Топливо, л")),r,0);
    gl->addWidget(sbFuel,r,1);
    r++;
    gl->addWidget(new QLabel(tr("Название")),r,0);
    gl->addWidget(leName,r,1,1,4);
    r++;
    gl->addWidget(new QLabel(tr("cmt")),r,0);
    gl->addWidget(leCmt,r,1,1,4);
    r++;
    gl->addWidget(new QLabel(tr("desc")),r,0);
    gl->addWidget(leDesc,r,1,1,4);
    r++;
    gl->addWidget(new QLabel(tr("src")),r,0);
    gl->addWidget(leSrc,r,1,1,4);
    r++;
    gl->addWidget(new QLabel(tr("sym")),r,0);
    gl->addWidget(leSym,r,1,1,4);
    r++;
    gl->addWidget(new QLabel(tr("type")),r,0);
    gl->addWidget(leType,r,1,1,4);
    r++;
    gl->addWidget(chControl,r,1);
    r++;
    gl->addWidget(new QLabel(tr("Время прибытия(UTC)")),r,0);
    gl->addWidget(dteArr,r,1);
    gl->addWidget(tbCalcArr,r,2);
    r++;
    gl->addWidget(new QLabel(tr("Время отправления(UTC)")),r,0);
    gl->addWidget(dteDep,r,1);
    gl->addWidget(tbCalcDep,r,2);
    r++;

    mainlayout = new QVBoxLayout();
    mainlayout->addLayout(gl);
    mainlayout->addWidget(buttonBox);
    setLayout(mainlayout);

    connect(tbCalcArr, SIGNAL(clicked()), SLOT(calcArr()));
    connect(tbCalcDep, SIGNAL(clicked()), SLOT(calcDep()));

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

WptEditDlg::~WptEditDlg()
{

}

void WptEditDlg::setMarking(WptMrk *val)
{
    wptmrk = val;
    for (int i=0; i<wptmrk->rowCount(); i++)
    {
        cbMarking->addItem(QIcon(wptmrk->getImagePath(i)),wptmrk->getName(i), wptmrk->getMarking(i));
    }
}

void WptEditDlg::setWayPoint(WayPoint* val, int cf)
{
    p = val;
    gpointwdg->setGeoCoordinate(*p, static_cast<GeoPoint::DispFormat>(cf));
    dteTime->setDateTime(p->time);
    cbMarking->setCurrentIndex(wptmrk->markingIndex(p->marking()));
    sbFuel->setValue(p->fuel());
    leName->setText(p->name());
    leCmt->setText(p->cmt());
    leDesc->setText(p->desc());
    leSrc->setText(p->src());
    leSym->setText(p->sym());
    leType->setText(p->type());
    chControl->setChecked(p->control());
    dteArr->setDateTime(p->arr());
    dteDep->setDateTime(p->dep());
}

void WptEditDlg::accept()
{
    QDialog::accept();

    GeoPoint gc = gpointwdg->getGeoCoordinate();
    if(p->latitude != gc.latitude) p->latitude = gc.latitude;
    if(p->longitude != gc.longitude) p->longitude = gc.longitude;
    if(p->altitude != gc.altitude) p->altitude = gc.altitude;
    if(p->marking()!= cbMarking->currentData().toString())
    {
        p->setMarking(cbMarking->currentData().toString());
        wptmrk->setImageToWayPoint(p);
    }
    if(p->fuel()!=sbFuel->value()) p->setFuel(sbFuel->value());
    if(p->name()!=leName->text()) p->setName(leName->text());
    if(p->cmt()!=leCmt->text()) p->setCmt(leCmt->text());
    if(p->desc()!=leDesc->text()) p->setDesc(leDesc->text());
    if(p->src()!=leSrc->text()) p->setSrc(leSrc->text());
    if(p->sym()!=leSym->text()) p->setSym(leSym->text());
    if(p->type()!=leType->text()) p->setType(leType->text());
    if(p->control()!=chControl->isChecked()) p->setControl(chControl->isChecked());
    if(p->arr()!=dteArr->dateTime()) p->setArr(dteArr->dateTime());
    if(p->dep()!=dteDep->dateTime()) p->setDep(dteDep->dateTime());
}

void WptEditDlg::calcArr()
{
    int idx = gpx->findPointIndex(dteTime->dateTime());
    if(idx>=0)
    {
        QDateTime dt = gpx->calcArrTime(idx);
        dteArr->setDateTime(dt);
    }
}

void WptEditDlg::calcDep()
{
    int idx = gpx->findPointIndex(dteTime->dateTime());
    if(idx>=0)
    {
        QDateTime dt = gpx->calcDepTime(idx);
        dteDep->setDateTime(dt);
    }
}
