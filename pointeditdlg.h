#ifndef POINTEDITDLG_H
#define POINTEDITDLG_H

#include <QDialog>
#include "trackpoint.h"

class TrackPoint;
class QDialogButtonBox;
class GPointWdg;
class QVBoxLayout;
class QGridLayout;
class QDateTimeEdit;

class PointEditDlg : public QDialog
{
    Q_OBJECT
public:
    explicit PointEditDlg(QWidget *parent = 0);
    ~PointEditDlg();
    void setTrackPoint(TrackPoint* val, int cf);
    TrackPoint* getTrackPoint(){return p;}

signals:

public slots:
    virtual void accept ();
protected:
    TrackPoint* p;
    GPointWdg* gpointwdg;
    QDateTimeEdit* dteTime;
    QDialogButtonBox* buttonBox;
    QVBoxLayout* mainlayout;
    QGridLayout* gl;
};

#endif // POINTEDITDLG_H
