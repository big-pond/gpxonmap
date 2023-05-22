#include <QImage>
#include <QPixmap>
#include <QTimer>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkDiskCache>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslSocket>

#include "tilereader.h"

//#include <QDebug>

TileReader::TileReader(const QString &cdname, QObject *parent)
    : QObject{parent}
{
    /*  Если появляются ошибки:
     *
     *  qt.network.ssl: QSslSocket: cannot call unresolved function SSLv23_client_method
     *  qt.network.ssl: QSslSocket: cannot call unresolved function SSL_CTX_new
     *  qt.network.ssl: QSslSocket: cannot call unresolved function SSL_library_init
     *  qt.network.ssl: QSslSocket: cannot call unresolved function ERR_get_error
     *  qt.network.ssl: QSslSocket: cannot call unresolved function ERR_get_error
     *  и тайлы не скачиваются
     *
     * Скачать с http://wiki.overbyte.eu/wiki/index.php/ICS_Download подходящую версию OpenSsl,
     * узнав ее из QSslSocket::sslLibraryBuildVersionString()
     * поместить файлы libeay32.dll и and ssleay32.dll в папку, в которую собирается *.exe и
     * лежат *.o
     */
//    qDebug() << "OpenSSL version: " << QSslSocket::sslLibraryBuildVersionString();
    cache = new QNetworkDiskCache(this);
    cache->setCacheDirectory(cdname);
    manager = new QNetworkAccessManager(this);
    manager->setCache(cache);
    request = new QNetworkRequest();
    request->setRawHeader(QByteArray("User-Agent"), QByteArray("GPX on map")); // request.setRawHeader("User-Agent", "Nokia (Qt) Graphics Dojo 1.0");
    timer = new QTimer(this);
    cacheloadcontrol = QNetworkRequest::PreferCache;
    // Скачивание  по  сигналу  таймера
    connect(timer, SIGNAL(timeout()), this, SLOT(download()));
    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(handleNetworkReply(QNetworkReply *)));
}

TileReader::~TileReader()
{
   delete request;
}

void TileReader::setTimeout(int ms)
{
    timer->start(ms);
}

void TileReader::setCacheSize(int size)
{
    cache->setMaximumCacheSize(size*1024*1024);
    //    qDebug() << "cachesize" << cache->maximumCacheSize();
}

void TileReader::setCacheloadcontrol(int clc)
{
    cacheloadcontrol = static_cast<QNetworkRequest::CacheLoadControl>(clc);
}

void TileReader::setUrlBase(const QString& ub)
{
    urlbase = ub;
}

void TileReader::download()
{
    if(queue.count()>0)
    {
        int x,y,z;
        QString key = queue.dequeue();
        posFromKey(key, x, y, z);
        QUrl url = getTileURL(x, y, z);
//        qDebug() << url.url();
        request->setUrl(url);  //формируем запрос
        request->setAttribute(QNetworkRequest::User, QVariant(key));
        request->setAttribute(QNetworkRequest::CacheLoadControlAttribute, cacheloadcontrol);
        manager->get(*request); //отправляем запрос
    }
}

void TileReader::handleNetworkReply(QNetworkReply* reply)
{
    QImage img;
    QString key = reply->request().attribute(QNetworkRequest::User).toString();
    if (!reply->error())   {
        if (!img.load(reply, 0))
            img = QImage();
    }
    else
        qDebug() << "err" << reply->error();
    reply->deleteLater();

    if (!img.isNull())
    {
        QPixmap pxm = QPixmap::fromImage(img);
        tiles->insert(key,pxm);    // вместо  saveTileToCache(key, pxm);
        int x,y,z;
        posFromKey(key, x, y, z);
        emit sendUpdateTileRect(x, y, z);
    }
}

void TileReader::placeKeyTileToQueue(const int& x, const int& y, const int& z)
{
    QString key = QString("%1&%2&%3").arg(x).arg(y).arg(z);
    queue.enqueue(key);
}

QString TileReader::getTileURL(const int& x, const int& y, const int& z)
{
    return QString("%1/%2/%3/%4.png").arg(urlbase).arg(z).arg(x).arg(y);
}

void TileReader::posFromKey(const QString &key, int &x, int &y, int &z)
{
    x = key.section('&',0,0).toInt();
    y = key.section('&',1,1).toInt();
    z = key.section('&',2,2).toInt();
}
