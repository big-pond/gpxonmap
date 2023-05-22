#ifndef COLORRECT_H
#define COLORRECT_H

#include <QWidget>

class ColorRect : public QWidget
{
    Q_OBJECT
public:
    ColorRect(QWidget *parent = 0);
    ColorRect(const int width, const int height, QWidget *parent = 0);
    ColorRect(const int width, const int height, const QColor& c, QWidget *parent = 0);
    bool isChangedColor(){return ischanged;}
    void setChange(bool val){change=val;}
signals:
    void changeColor(QColor);
public slots:
    void setColor(const QColor& c){color = c; ischanged = false;}
    QColor getColor(){return color;}
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent ( QMouseEvent * event );
private:
    QColor color;
    bool ischanged;
    bool change; //можно ли менять цвет
};

#endif // COLORRECT_H
