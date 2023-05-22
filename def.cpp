#include "def.h"

const QString Def::WGS84 = "WGS 84";
const QString Def::KRASOVSKY = "Krasovsky";
const QString Def::PE90 = "PE 90";
const QString Def::S6371000 = "Sph R=6371000";
const QString Def::S6378137 = "Sph R=6378137";


const QString Def::TRANSMERCATOR = "Transverse Mercator";  // Поперечная  Меркатора, является синонимом Гаусса-Крюгера
const QString Def::GAUSS_KRUGER = "Gauss-Kruger";
const QString Def::MERCATOR = "Mercator";

QString Def::gpxdir = "./gpx";

const QStringList Def::default_speed_colors = QStringList() << "#ff0000" << "#ff8000" << "#ffc000"
                                                            << "#ffff00" << "#c0ff00" << "#80ff00"
                                                            << "#00c000" << "#00a000" << "#00c080"
                                                            << "#00c0ff" << "#0080ff" << "#0040ff"
                                                            << "#0000ff" << "#4000ff" << "#8000ff"
                                                            << "#c000ff" << "#ff00ff";

Def::Def()
{

}
