//Copyright (C) Yuriy Popov 2012
//Contact: Yuriy Popov (urmingard@yandex.ru)
//This file is part of application TripOnMap

#ifndef ELLIPSOID_H
#define ELLIPSOID_H

class Ellipsoid
{
public:

    Ellipsoid();
    // _a - большая полуось, p - малая полуось или полярное сжатие
    Ellipsoid(const double& _a, const double& p);
    void setData(const double& _a, const double& p);
    double getA(){return a;}
    double getB(){return b;}
    double getAlpha(){return alpha;}
    double getEps(){return eps;}
    double getEps2(){return eps2;}
    double getEps_(){return eps_;}
    double getEps_2(){return eps_2;}
    //радиус кривизны первого вертикала, B - широта,рад
    double N(const double& B);
    //радиус кривизны меридиана, B - широта,рад
    double M(const double& B);
    //средний радиус кривизны
    double mR(const double& B);
    //Преобразование геодезических координат в пространственные прямоугольные (м)
    //широта, долгота  - рад, высота h - м
    void blhrad_to_xyz(double& x, double& y, double& z,
                 const double& br,  const double& lo, const double& h);
    //Преобразование пространственных прямоугольных координат(м) в геодезические
    //широта, долгота  - рад, высота h - м
    void xyz_to_blhrad(const double& x, const double& y, const double& z,
                    double& br,  double& lo, double& h);
    //Преобразование геодезических координат в пространственные прямоугольные (м)
    //широта, долгота  - градусы, высота h - м
    void blhdeg_to_xyz(double& x, double& y, double& z,
                 const double& br,  const double& lo, const double& h);
    //Преобразование пространственных прямоугольных координат(м) в геодезические
    //широта, долгота  - градусы, высота h - м
    void xyz_to_blhdeg(const double& x, const double& y, const double& z,
                    double& br,  double& lo, double& h);
    //Короткие расстояния между точками по прямой
    //Величины линейных искажений при замене сферы R=6371км плоскостью
    //Г.Г.Поклад,С.П.Гриднев.Геодезия.Учеб.пособ.для вузов.-М,2007.
    //s,км   10     15     20      25      50
    //ds,м  0,008  0,028  0,066   0,13    1,02
    //долгота и широта в градусах
    double getDistanceLn(const double& b1, const double& l1, const double& h1,
                         const double& b2, const double& l2, const double& h2);
    //расстояние по эллипсоиду
    double getDistanceEl(const double& b1, const double& l1,
                         const double& b2, const double& l2);

    //РЕШЕНИЕ обратной геодезической задачи (Vincenty's_formulae)
    //Входные параметры широта и долгота двух точек (b1, l1) и (b2, l2), градусы
    //Выходные параметры азимуты az1, az2, градусы
    //Возвращает расстояние между точками по эллипсоиду в метрах
    double getDistanceV(const double& b1, const double& l1,
                        const double& b2, const double& l2,
                        double& az1, double& az2);
    //РЕШЕНИЕ обратной геодезической задачи (Vincenty's_formulae)
    //Входные параметры широта и долгота двух точек (b1, l1) и (b2, l2), градусы
    //Возвращает расстояние между точками по эллипсоиду в метрах, азимуты не рассчитываются
    double getDistanceV(const double& b1, const double& l1,
                        const double& b2, const double& l2);
    double getDistanceV1(const double& b1, const double& l1, const double& h1,
                         const double& b2, const double& l2, const double& h2);
    //РЕШЕНИЕ прямой геодезической задачи (Vincenty's_formulae)
    //Входные параметры широта и долгота точки (b1, l1), расстояние s и азимут az1
    //Выходные параметры широта и долгота точки (b2, l2) и азимут az2
    //b1, b2, l1, l2, az1, az2 в градусах, s в метрах
    void getPointV(const double& b1, const double& l1,const double& s, const double& az1,
                   double& b2, double& l2, double& az2);
    double getEquatorLength();
    double getParalleLengthOnLatRad(const double &lat);
    double getParalleLengthOnLatDeg(const double &lat);
private:
    double a;     //Большая полуось эллипсоида
    double b;     //Малая полуось эллипсоида
    double alpha; //Полярное сжатие
    double eps;   //Эксцентриситет
    double eps2;  //Квадрат эксцентриситета
    double eps_;  //Второй эксцентриситет
    double eps_2; //Квадрат второго эксцентриситета
};

#endif // ELLIPSOID_H
