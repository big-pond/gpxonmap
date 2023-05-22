#ifndef TILEREADER_H
#define TILEREADER_H

#include <QObject>
#include <QQueue>
#include <QVariant>

class QTimer;
class QNetworkAccessManager;
class QNetworkDiskCache;
class QNetworkReply;
class QNetworkRequest;

class TileReader : public QObject
{
    Q_OBJECT
public:
    explicit TileReader(const QString& cdname, QObject *parent = nullptr);
    ~TileReader();

    void setTiles(QHash<QString, QPixmap> *val){tiles = val;}
    void setTimeout(int ms);
    void setUrlBase(const QString &ub);
    QString getTileURL(const int &x, const int &y, const int &z);
    void placeKeyTileToQueue(const int &x, const int &y, const int &z);
    static void posFromKey(const QString& key, int& x, int& y, int& z);
    void setCacheSize(int size);
    void setCacheloadcontrol(int clc);
    QVariant getCacheloadcontrol(){return cacheloadcontrol;}
//    bool isTileExist(int x, int y, int z);
public slots:
    void download();
    void handleNetworkReply(QNetworkReply *reply);
signals:
    void sendUpdateTileRect(int x, int y, int z);
protected:
    QHash<QString, QPixmap> *tiles;
    QQueue<QString> queue; //очередь на закачку

    QString urlbase;
    //для скачивания тайлов
    QTimer* timer;
//    int timeout;

    QNetworkDiskCache *cache;
    QNetworkAccessManager* manager;
    QVariant cacheloadcontrol;
    QNetworkRequest* request;
};

#endif // TILEREADER_H
