#include "scalewdg.h"
#include <math.h>
#include <QPainter>

ScaleWdg::ScaleWdg(QWidget *parent) :
    QWidget(parent)
{
    h_del = 8;
    xstart = 5;
    y_l = 28;
    setGeometry(0,0,120,y_l+4);
}
void ScaleWdg::setScale(const double& mtop/*, const double& scale*/)
{
    mtopix = mtop/*/scale*/;
    double m_in_100pix = mtopix*100;
    double pst = floor(log10(m_in_100pix)); //показатель степени
    double x = m_in_100pix/pow(10, pst); //x (10;1)
    if(x>=10.0) m = 10.0;
    else if(x<10.0&&x>=7.5) m = 7.5;
    else if(x<7.5&&x>=5.0) m = 5.0;
    else if(x<5.0&&x>=2.5) m = 2.5;
    else if(x<2.5&&x>=1.0) m = 1.0;
    m *= pow(10,pst);
    l = m/mtopix;

    setFixedWidth(l+40);
    if(m>=5000)
    {
        sm = tr("%1 км").arg(m*0.001,0,'f',1);
    }
    else
    {
        sm = tr("%1 м").arg(m,0,'f',0);
    }
}

/*virtual*/
void ScaleWdg::paintEvent ( QPaintEvent * /*event*/ )
{
    QPainter painter;
    painter.begin(this);
    painter.setPen(Qt::white);
    painter.setBrush(QBrush(Qt::white));
    QRect r = painter.fontMetrics ().boundingRect(sm);
    painter.setOpacity(0.7);
    painter.drawRect(0,0,xstart+l+r.width()/2, y_l+4);
    painter.setOpacity(1);
    painter.setPen(Qt::black);
    painter.drawLine(xstart,y_l,xstart + l ,y_l);
    painter.drawLine(xstart,y_l,xstart,y_l-h_del);//первое деление
    painter.drawLine(xstart+l,y_l,xstart+l,y_l-h_del);//второе деление
    painter.drawText(xstart, y_l-r.height()-2, QChar('0'));
    painter.drawText(xstart+l-r.width()/2, y_l-r.height()-2, sm);
    painter.end();
}
