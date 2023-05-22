#ifndef WPTEDITDLG_H
#define WPTEDITDLG_H

#include <QDialog>

class QDialogButtonBox;
class GPointWdg;
class QVBoxLayout;
class QGridLayout;
class QDateTimeEdit;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QToolButton;
class WayPoint;
class Gpx;
class WptMrk;

class WptEditDlg : public QDialog
{
    Q_OBJECT
public:
    explicit WptEditDlg(QWidget *parent = 0);
    ~WptEditDlg();
    void setMarking(WptMrk* val);
    void setGpx(Gpx* val){gpx = val;}
    void setWayPoint(WayPoint* val, int cf);
    WayPoint* getWayPoint(){return p;}

signals:

public slots:
    virtual void accept ();
protected slots:
    void calcArr();
    void calcDep();
protected:
    WayPoint* p;
    WptMrk* wptmrk;
    Gpx* gpx;
    GPointWdg* gpointwdg;
    QDateTimeEdit* dteTime;
    QComboBox* cbMarking;
    QDoubleSpinBox* sbFuel;
    QLineEdit* leName;
    QLineEdit* leCmt;
    QLineEdit* leDesc;
    QLineEdit* leSrc;
    QLineEdit* leSym;
    QLineEdit* leType;
    QCheckBox* chControl;
    QDateTimeEdit* dteArr;
    QDateTimeEdit* dteDep;
    QToolButton* tbCalcArr;
    QToolButton* tbCalcDep;

    QDialogButtonBox* buttonBox;

    QVBoxLayout* mainlayout;
    QGridLayout* gl;
};

#endif // WPTEDITDLG_H
