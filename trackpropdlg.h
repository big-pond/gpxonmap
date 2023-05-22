#ifndef TRACKPROPDLG_H
#define TRACKPROPDLG_H

#include <QDialog>
#include "colorrect.h"

class QLabel;
class QLineEdit;
class QTableWidget;
class QSpinBox;
//class QDoubleSpinBox;
class QCheckBox;
class QGroupBox;
class QComboBox;
class QDialogButtonBox;
class Gpx;
class ColorRect;

class TrackPropDlg : public QDialog
{
    Q_OBJECT
public:
    explicit TrackPropDlg(QWidget *parent = 0);
    void setGpx(Gpx* t);
public slots:
    virtual void accept ();
//иногда трек не отображается из-за "неправильной рамки"
    void updateTrackBounds(bool);
protected:
    Gpx* gpx;
    QLineEdit* leName;
    QLabel* lbFileName, *lbTabTit1, *lbTabTit2;
    QSpinBox* sbTimeOffset;
    QSpinBox* sbLineWidth;
    QCheckBox* cbxLineVisible;
    QSpinBox* sbPointSize;
    QCheckBox* cbxPointVisible;
    ColorRect* color;
    QCheckBox* cbxSpeedVisible;

    QTableWidget* twInfo;
    QTableWidget* twInfo1;
    QDialogButtonBox* buttonBox;
    void calcTrackProperty();
    void addRowToTabl(QTableWidget* tw,const int& row, const QString& nm, const double& dist, const qint64& time);
protected slots:
    void changePrefix(int val);
};

#endif // TRACKPROPDLG_H
