#include <math.h>
#include "util.h"

const double DEG_IN_RAD = 57.29577951308232087680; // 1/PI*180
const double RAD_IN_DEG = 0.01745329251994329577;  // 1/180*PI

const double LOWEST = -427;
const double HIGHEST = 8848;
//const double SPEED_STOP = 0.1; //км/ч

int pow2(int n)  //Возводит 2 в степень n
{
    return (1 << n);
}

double pow2f(int n)  //Возводит 2 в степень n
{
    return (static_cast<double>(1<<n));
}

double degrees(const double& x)
{
    return x*DEG_IN_RAD;
}

double radians(const double& x)
{
    return x*RAD_IN_DEG;
}

//Формиравание ключа по позиции тайла
QString defineKey(const int x, const int y, const int z)
{
    return QString("%1&%2&%3").arg(z).arg(y).arg(x);
}
//Определение позиции тайла по ключу
void posFromKey(const QString& key, int& x, int& y, int& z)
{
    z = key.section('&',0,0).toInt();
    y = key.section('&',1,1).toInt();
    x = key.section('&',2,2).toInt();
}

void degToDMS(const double& deg, double& g, double& m, double& s)
{
    double integer, fraction;
    double grd = fabs(deg);
    fraction = modf(grd, &integer);
    g = integer;
    fraction = modf(fraction*60, &integer);
    m = integer;
    s = fraction*60;
    if(fabs(s-60)<1e-4){ m++; s=0;}
    if(s<1.e-4) s=0;
}

void degToDM(const double& deg, double& g, double& m)
{
    double integer, fraction;
    double grd = fabs(deg);
    fraction = modf(grd, &integer);
    g = integer;
    m = fraction*60;
    if(fabs(m-60)<1.e-7){g++; m=0;}
    if(m<1.e-7) m = 0;
}

void msecToHMS(const qint64& msec, int& h, int& m, int& s, int& ms)
{
    h = msec/3600000;
    m = (msec - h*3600000)/60000;
    s = (msec - h*3600000 - m*60000)/1000;
    ms = msec - h*3600000 - m*60000 - s*1000;
}

QString msecToHMS(const qint64& msec)
{
    QChar z('0');
    int h, m, s, ms;
    msecToHMS(msec, h, m, s, ms);
    return QString("%1:%2:%3").arg(h).arg(m,2,10,z).arg(s,2,10,z);
}

void secToHMS(const qint64 &sec, int &h, int &m, int &s)
{
    h = sec/3600;
    m = (sec - h*3600)/60;
    s = sec - h*3600 - m*60;
}

QString secToHMS(const qint64 &sec)
{
    QChar z('0');
    int h, m, s;
    secToHMS(sec, h, m, s);
    return QString("%1:%2:%3").arg(h).arg(m,2,10,z).arg(s,2,10,z);
}
