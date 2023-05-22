#ifndef MAPDESCRIPTION_H
#define MAPDESCRIPTION_H

#include <QString>

class MapDescription
{
public:
    MapDescription();
    void setName(const QString& val) {name = val;}
    QString getName(){return name;}
    void setEllipsoid(const QString& val) {ellipsoid = val;}
    QString getEllipsoid(){return ellipsoid;}
    void setProjection(const QString& val) {projection = val;}
    QString getProjection(){return projection;}
    void setUrlBase(const QString& val) {urlbase = val;}
    QString getUrlbase(){return urlbase;}
    void setTileSource(int val){tile_source =val;}
    int getTileSource(){return tile_source;}
    void setTimeout(int val) {timeout = val;}
    int getTimeout(){return timeout;}
    void setCacheSize(int val) {cache_size = val;}
    int getCacheSize(){return cache_size;}  // размер кэша в мегабайтах
    void setZmin(int val) {z_min = val;}
    int getZmin(){return z_min;}
    void setZmax(int val) {z_max = val;}
    int getZmax(){return z_max;}
    void setZ(int val) {z = val;}
    int getZ(){return z;}
    void setLat(double val) {lat = val;}
    double getLat(){return lat;}
    void setLon(double val) {lon = val;}
    double getLon(){return lon;}
    void setWidth(int val) {width = val;}
    int getWidth(){return width;}
    void setHeight(int val) {height = val;}
    int getHeight(){return height;}
    virtual QString url(int x, int y, int z) = 0;
protected:
    QString name;
    QString ellipsoid;
    QString projection;
    QString urlbase;
    int tile_source;  // QNetworkRequest::PreferCache
    int timeout;
    int cache_size;  // размер кэша в мегабайтах
    int z_min;
    int z_max;
    int z;
    double lat;
    double lon;
    int width;  // width map widget
    int height;  // height map widget

};

class OSMDescription : public MapDescription
{
public:
    OSMDescription();
    QString url(int x, int y, int z);
};

class OTMDescription : public MapDescription
{
public:
    OTMDescription();
    QString url(int x, int y, int z);
};

#endif // MAPDESCRIPTION_H
