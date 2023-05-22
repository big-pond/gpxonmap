#include "movetodlg.h"
#include <QComboBox>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>

MoveToDlg::MoveToDlg(QWidget *parent) : QDialog(parent)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    gpointwdg = new GPointWdg(this);
    gpointwdg->setAltVisible(false);
    cbLevels = new QComboBox();
    for(int i = 1; i<21; i++)
        cbLevels->addItem(QString("z %1").arg(i));

    QGridLayout* gl = new QGridLayout();
    gl->setSpacing(4);
    QLabel* lb = new QLabel(tr("Level")); lb->setAlignment(Qt::AlignRight);
    gl->addWidget(lb,0,0);
    gl->addWidget(cbLevels,0,1);
    gl->addWidget(new QWidget(),0,2);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QVBoxLayout* vl = new QVBoxLayout();
    vl->setSpacing(4);
    vl->setContentsMargins(8,1,8,1);
    vl->addWidget(gpointwdg);
    vl->addItem(gl);
    vl->addWidget(buttonBox);
    setLayout(vl);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    setWindowTitle(tr("Move to point"));
}

void MoveToDlg::setData(const GeoPoint &gc, int level, GeoPoint::DispFormat f)
{
    if ( f==GeoPoint::DegFloat || f==GeoPoint::RadFloat || f==GeoPoint::Metre || f==GeoPoint::Pixel)
        f = GeoPoint::Deg;
    gpointwdg->setGeoCoordinate(gc, f);
    cbLevels->setCurrentIndex(level);
}

void MoveToDlg::getData(GeoPoint &gc, int& level)
{
    gc = gpointwdg->getGeoCoordinate();
    level = cbLevels->currentIndex();
}

void MoveToDlg::accept()
{
    QDialog::accept();
}
