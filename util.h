#ifndef UTIL
#define UTIL
#include <QString>

extern const double DEG_IN_RAD;
extern const double RAD_IN_DEG;

extern const double LOWEST;
extern const double HIGHEST;
//extern const double SPEED_STOP;

int pow2(int n);  //Возводит 2 в степень n
double pow2f(int n);  //Возводит 2 в степень n

double radians(const double& x);
double degrees(const double& x);

//Формиравание ключа по позиции тайла
QString defineKey(const int x, const int y, const int z);
//Определение позиции тайла по ключу
void posFromKey(const QString& key, int& x, int& y, int& z);

void degToDMS(const double& deg, double &g, double &m, double& s);
void degToDM(const double& deg, double &g, double& m);

void msecToHMS(const qint64& msec, int& h, int& m, int& s, int& ms);
QString msecToHMS(const qint64& msec);
void secToHMS(const qint64& sec, int& h, int& m, int& s);
QString secToHMS(const qint64& sec);

#endif // UTIL

