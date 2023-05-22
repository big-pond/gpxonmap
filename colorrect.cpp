#include "colorrect.h"
#include <QColorDialog>
#include <QPainter>

ColorRect::ColorRect(QWidget *parent)
    : QWidget(parent), color(Qt::white),ischanged(false),change(true)
{
    this->setGeometry(0,0,15, 15);
}
ColorRect::ColorRect(const int width, const int height, QWidget *parent)
    : QWidget(parent), color(Qt::white),ischanged(false),change(true)
{
    this->setGeometry(0,0,width, height);
}

ColorRect::ColorRect(const int width, const int height, const QColor& c,
                     QWidget *parent)
    : QWidget(parent), color(c),ischanged(false),change(true)
{
    this->setGeometry(0,0,width, height);
}

void ColorRect::paintEvent(QPaintEvent* )
{
    QPainter painter(this);
    painter.setBrush(QBrush(color));
    painter.drawRect(0,0,width()-1,height()-1);
}
void ColorRect::mousePressEvent ( QMouseEvent * event )
{
    QWidget::mousePressEvent(event);
    if(!change) return;
    QColor newcolor = QColorDialog::getColor(color);
    if(newcolor.isValid())
    {
       color = newcolor;
       ischanged = true;
       emit changeColor(color);
    }
}

