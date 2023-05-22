#ifndef WEATHERREADER_H
#define WEATHERREADER_H

#include <QObject>
#include <QMap>

class QNetworkAccessManager;

class WeatherReader : public QObject
{
    Q_OBJECT
public:
    explicit WeatherReader(QObject *parent = nullptr);
    static QString w_token;
    static QString dir_wind(const double& dir);
    void getWeather(const double &lat, const double &lon);
    void getWeather(const QString &city_name);
//    QString getResult() {return result;}
signals:
    void resultReceived(const QString& text);
public slots:
    void replyFinished();
private:
    QNetworkAccessManager* manager;
    QMap<QString, QString> smile_codes;
//    QString result;
};

#endif // WEATHERREADER_H
