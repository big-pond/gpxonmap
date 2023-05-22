#ifndef GCRECALC_H
#define GCRECALC_H
#include <QString>

class Ellipsoid;

class GCRecalc
{
public:
    GCRecalc(/*const QString& nm,*/ Ellipsoid* _el, Ellipsoid* el0,
          const double& _dx, const double& _dy, const double& _dz,
          const double& _wx, const double& _wy, const double& _wz,
          const double& _ms);
    //-----Методы для пересчета широты и долготы.   br, lo - радианы, h - метры
    void blh_A_to_B(double &br, double &lo, double &h);
    void blh_B_to_A(double &br, double &lo, double &h);
    //-----Методы для пересчета пространственных прямоугольных координат - метры
    void xyz_A_to_B(double &x_, double &y_, double &z_);
    void xyz_B_to_A(double &x_, double &y_, double &z_);
protected:
    static const double ro; //Число угловых секунд в радиане
    static const double rs; //Число радиан в угловой секунде

//    QString name;
    Ellipsoid* el;
    //----Линейные элементы трансформирования, в метрах--------------------------
    double dx;
    double dy;
    double dz;
    //----Угловые элементы трансформирования, в секундах-------------------------
    double wx;
    double wy;
    double wz;
    //----Дифференциальное различие масштабов------------------------------------
    double ms;

    //----Вспомогательные значения эллипсоидов для преобразования ----------------
    double a;
    double e2;
    double da;
    double de2;

    double N;//Радиус кривизны первого вертикала. Зависит от широты, вычисляется каждый раз
    double dB(const double &B, const double &L,  const double &h); //B, L - радианы, h - метры
    double dL(const double &B, const double &L,  const double &h); //B, L - радианы, h - метры
    double dH(const double &B, const double &L,  const double &h); //B, L - радианы, h - метры
};

#endif // GCRECALC_H
