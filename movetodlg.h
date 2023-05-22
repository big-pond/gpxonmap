#ifndef MOVETODLG_H
#define MOVETODLG_H

#include <QDialog>
#include "gpointwdg.h"

class QDialogButtonBox;
class QComboBox;

class MoveToDlg : public QDialog
{
    Q_OBJECT
public slots:
    virtual void accept ();
public:
    explicit MoveToDlg(QWidget *parent = 0);
    void setData(const GeoPoint& gc, int level,
                 GeoPoint::DispFormat f=GeoPoint::Deg);
    void getData(GeoPoint& gc, int& level);
    
private:
    GPointWdg* gpointwdg;
    QComboBox* cbLevels;
    QDialogButtonBox* buttonBox;
};

#endif // MOVETODLG_H
