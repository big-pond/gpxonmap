#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QTextCodec>

#include "weatherreader.h"

QString WeatherReader::w_token = "6fcbdad034343de2e7304226155449e4";

WeatherReader::WeatherReader(QObject *parent) : QObject(parent)
{
//    smile_codes["Clear"] = "ясно \U00002600";
//    smile_codes["Clouds"] = "облачно \U00002601";
//    smile_codes["Rain"] = "дождь \U00002614";
//    smile_codes["Drizzle"] = "дождь \U00002614";
//    smile_codes["Thunderstorm"] = "гроза \U000026A1";
//    smile_codes["Snow"] = "снег \U0001F328";
//    smile_codes["Mist"] = "туман \U0001F32B";

    smile_codes["Clear"] = " \U00002600";
    smile_codes["Clouds"] = " \U00002601";
    smile_codes["Rain"] = " \U00002614";
    smile_codes["Drizzle"] = " \U00002614";
    smile_codes["Thunderstorm"] = " \U000026A1";
    smile_codes["Snow"] = " \U0001F328";
    smile_codes["Mist"] = " \U0001F32B";

    manager = new QNetworkAccessManager(this);

}

QString WeatherReader::dir_wind(const double &dir)
{
    QString str_dir = "Неизвестное направление";
    if (((dir >=337.5)&&(dir < 360)) || ((dir >= 0)&&(dir < 22.5)))
        str_dir = "Северный";
    else if ((dir >= 22.5)&&(dir < 67.5))   //  22.5 <= dir <  67.5
        str_dir = "Северо-восточный";
    else if ((dir >= 67.5)&&(dir < 112.5))  //  67.5 <= dir < 112.5
        str_dir = "Восточный";
    else if ((dir >= 112.5)&&(dir < 157.5)) // 112.5 <= dir < 157.5
        str_dir = "Юго-восточный";
    else if ((dir >= 157.5)&&(dir < 202.5)) // 157.5 <= dir < 202.5
        str_dir = "Южный";
    else if ((dir >= 202.5)&&(dir < 247.5)) // 202.5 <= dir < 247.5
        str_dir = "Юго-западный";
    else if ((dir >= 247.5)&&(dir < 292.5)) // 247.5 <= dir < 292.5
        str_dir = "Западный";
    else if ((dir >= 292.5)&&(dir < 337.5)) // 292.5 <= dir < 337.5
        str_dir = "Северо-западный";
    return str_dir;
}

void WeatherReader::getWeather(const double& lat, const double& lon)
{
    QUrl url(QString("https://api.openweathermap.org/data/2.5/"
                     "weather?lat=%1&lon=%2&appid=%3&units=metric&lang=ru"
                     ).arg(lat,0,'f',4).arg(lon,0,'f',4).arg(w_token)
             );

    QNetworkRequest request(url);
    QNetworkReply* reply = manager->get(request);
    connect( reply, SIGNAL(finished()), this, SLOT(replyFinished()) );
}

void WeatherReader::getWeather(const QString &city_name)
{
    QUrl url(QString("https://api.openweathermap.org/data/2.5/"
                     "weather?q=%1&appid=%2&units=metric&lang=ru"
                     ).arg(city_name).arg(w_token)
             );
    QNetworkRequest request(url);
    QNetworkReply* reply = manager->get(request);
    connect( reply, SIGNAL(finished()), this, SLOT(replyFinished()) );
}

void WeatherReader::replyFinished()
{
  QString result;
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (reply->error() == QNetworkReply::NoError) {
     QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
     QJsonObject jsonobj = document.object();
//     qDebug() << jsonobj;
     QJsonValue value = jsonobj.value(QString("name"));
     result = tr("<strong>The weather in: %1</strong><br>").arg(value.toString());

     value = jsonobj.value(QString("coord"));
     QJsonObject jsonobj1 = value.toObject();
     double lat = jsonobj1.value(QString("lat")).toDouble();
     double lon = jsonobj1.value(QString("lon")).toDouble();
     result += tr("lat: %1 lon: %2<br>").arg(lat,0,'f',4).arg(lon,0,'f',4);

     value = jsonobj.value(QString("timezone"));
     int offsetSeconds= value.toInt(); //в секундах

     value = jsonobj.value(QString("clouds"));
     jsonobj1 = value.toObject();
     int clouds = jsonobj1.value(QString("all")).toInt();
     result += tr("clouds: %1\%<br>").arg(clouds);

     value = jsonobj.value(QString("visibility"));
     double visibility = value.toDouble()/1000; //видимость км

     value = jsonobj.value(QString("main"));
//     qDebug() << value;
     jsonobj1 = value.toObject();
     QJsonValue value1 = jsonobj1.value(QString("temp"));
     double temp = value1.toDouble();
     value1 = jsonobj1.value(QString("temp_max"));
     double temp_max = value1.toDouble();
     value1 = jsonobj1.value(QString("temp_min"));
     double temp_min = value1.toDouble();
     value1 = jsonobj1.value(QString("feels_like"));
     double feels_like = value1.toDouble();
     result += tr("Temperature: %1°C,  min: %2°C, max: %3°C, feels like: %4°C<br>")
             .arg(temp,0,'f',1)
             .arg(temp_min,0,'f',1)
             .arg(temp_max,0,'f',1)
             .arg(feels_like,0,'f',1);
     value1 = jsonobj1.value(QString("humidity"));
     result += tr("Humidity: %1\%<br>").arg(value1.toDouble(),0,'f',1);
     value1 = jsonobj1.value(QString("pressure"));
     result += tr("Pressure: %1 mm<br>").arg(value1.toDouble()*0.750062,0,'f',1);
     result += tr("Visibility: %1 km<br>").arg(visibility,0,'f',1);

     value = jsonobj.value(QString("wind"));
     jsonobj1 = value.toObject();
     value1 = jsonobj1.value(QString("speed"));
     result += tr("Wind speed: %1 m/s (%2 km/h)<br>").arg(value1.toDouble(),0,'f',2).arg(value1.toDouble()*3.6,0,'f',2);
     value1 = jsonobj1.value(QString("gust"));
     result += tr("Wind gust: %1 m/s<br>").arg(value1.toDouble(),0,'f',2);
     value1 = jsonobj1.value(QString("deg"));
     result += tr("Wind deg: %1°, %2<br>").arg(value1.toDouble(),0,'f',0).arg(dir_wind(value1.toDouble()));

     value = jsonobj.value(QString("sys"));
     jsonobj1 = value.toObject();
     value1 = jsonobj1.value(QString("sunrise"));
     QDateTime t1 = QDateTime::fromSecsSinceEpoch(value1.toInt(), Qt::OffsetFromUTC, offsetSeconds);
     result += tr("Sunrise: %1<br>").arg(t1.toString("hh:mm:ss"));
     value1 = jsonobj1.value(QString("sunset"));
     QDateTime t2 = QDateTime::fromSecsSinceEpoch(value1.toInt(), Qt::OffsetFromUTC, offsetSeconds);
     result += tr("Sunset: %1<br>").arg(t2.toString("hh:mm:ss"));
     int secs = t1.secsTo(t2);
     QTime t = QTime(0,0);
     result += tr("Day length: %1<br>").arg(t.addSecs(secs).toString("h:mm:ss"));

     value = jsonobj.value(QString("weather"));
//     qDebug() << value.isArray() << value.isObject();
     QJsonArray jsonarr = value.toArray();
     value1 = jsonarr.at(0);
     jsonobj1 = value1.toObject();
     QJsonValue value2 = jsonobj1.value(QString("description"));
     result += tr("Description: %1 ").arg(value2.toString());
     value2 = jsonobj1.value(QString("main"));
     if (smile_codes.contains(value2.toString()))
        result += tr("%1<br>").arg(smile_codes[value2.toString()]);
     else
        result += tr("<br>");
  }
  else {
    result = reply->errorString();
  }
  reply->deleteLater();
  emit resultReceived(result);
}

