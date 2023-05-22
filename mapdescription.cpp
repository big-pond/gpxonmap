#include "mapdescription.h"
#include "def.h"

MapDescription::MapDescription()
    : name("No name"), ellipsoid(), projection(), urlbase(),
    tile_source(2),  timeout(100),  cache_size(50),  z_min(0),  z_max(19),  z(0),
    lat(0.0), lon(0.0), width(256), height(256)
{

}

OSMDescription::OSMDescription()
    : MapDescription()
{
    name = "OSM";
    ellipsoid = Def::S6378137;
    projection = Def::MERCATOR;
    urlbase = "https://b.tile.openstreetmap.org";
}

QString OSMDescription::url(int x, int y, int z)
{
    return QString("%1/%2/%3/%4").arg(urlbase).arg(z).arg(x).arg(y);
}

OTMDescription::OTMDescription()
    : MapDescription()
{
    name = "OTM";
    ellipsoid = Def::S6378137;
    projection = Def::MERCATOR;
    urlbase = "https://a.tile.opentopomap.org";
}

QString OTMDescription::url(int x, int y, int z)
{
    return QString("%1/%2/%3/%4").arg(urlbase).arg(z).arg(x).arg(y);
}
