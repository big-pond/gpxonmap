#ifndef DEF_H
#define DEF_H

#include <QString>
#include <QStringList>

class Def
{
public:
    static const QString WGS84;
    static const QString KRASOVSKY;
    static const QString PE90;
    static const QString S6371000;
    static const QString S6378137;

    static const QString TRANSMERCATOR;  // Поперечная  Меркатора, является синонимом Гаусса-Крюгера
    static const QString GAUSS_KRUGER;
    static const QString MERCATOR;

    static QString gpxdir;

    static const QStringList default_speed_colors;
    Def();

};

#endif // DEF_H
