#ifndef SCALEWDG_H
#define SCALEWDG_H

#include <QWidget>

class ScaleWdg : public QWidget
{
    Q_OBJECT
public:
    explicit ScaleWdg(QWidget *parent = 0);
    
public slots:
    void setScale(const double& mtop/*, const double& scale*/);
protected:
    virtual void paintEvent ( QPaintEvent * event );
    double mtopix;
    double m; //единица масштаба
    double l; //длина линии m метров в пикселях
    QString sm;
    int h_del; //высота деления шкалы (pix)
    int xstart; //координата x начала масштабной линии
    int y_l; //координата y масштабной линии
};

#endif // SCALEWDG_H
