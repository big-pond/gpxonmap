#include "gcrecalc.h"
#include "ellipsoid.h"

const double GCRecalc::ro = 206264.8062;
const double GCRecalc::rs = M_PI/648000.0;

GCRecalc::GCRecalc(/*const QString& nm,*/ Ellipsoid* _el, Ellipsoid* el0,
          const double& _dx, const double& _dy, const double& _dz,
          const double& _wx, const double& _wy, const double& _wz,
          const double& _ms)
{
//     name = nm;
     el = _el;
     double a1 =  el->getA();
     double a2 = el0->getA();
     double e2_1 =  el->getEps2();// 2.0 * alpha1 - alpha1*alpha1; //Квадрат эксцентриситета эллипсоида датума
     double e2_2 = el0->getEps2();//  2.0 * alpha2 - alpha2*alpha2;//Квадрат эксцентриситета эллипсоида WGS-84

     a = (a2 + a1) / 2.0;
     e2 = (e2_2 + e2_1) / 2.0;
     da = a2 - a1;
     de2  = e2_2 - e2_1;

     dx = _dx;
     dy = _dy;
     dz = _dz;

     wx = _wx;
     wy = _wy;
     wz = _wz;
     ms = _ms;
}

double GCRecalc::dB(const double &B, const double &L, const double &h)
{
    double M = a * (1 - e2) * pow(1 - e2 * sin(B)*sin(B), -1.5);
    return ro/(M + h)*(N/a*e2*sin(B)*cos(B)*da + (N*N /(a*a) + 1)*N*sin(B)*cos(B)
        * de2/2 - (dx*cos(L) + dy*sin(L))*sin(B) + dz*cos(B)) - wx*sin(L)
        * (1 + e2*cos(2*B)) + wy*cos(L)*(1 + e2*cos(2*B)) - ro*ms*e2*sin(B)*cos(B);
}

double GCRecalc::dL(const double &B, const double &L, const double &h)
{
    return  ro/((N + h)*cos(B))*(-dx*sin(L) + dy*cos(L)) + tan(B)*(1.0 - e2)
        * (wx*cos(L) + wy*sin(L)) - wz;
}

double GCRecalc::dH(const double &B, const double &L, const double &h)
{
    return -a/N*da + N*sin(B)*sin(B)*de2/2.0 + (dx*cos(L) + dy*sin(L))*cos(B)
       + dz*sin(B) - N*e2*sin(B)*cos(B)*(wx/ro*sin(L) - wy/ro*cos(L))
       + (a*a/N + h)*ms;
}

void GCRecalc::blh_A_to_B(double &br, double &lo, double &h)
{
    double B,L,H;
    B = br; L = lo; H = h;
    N = a / sqrt(1.0 - e2 * sin(B)*sin(B));
    br = B + dB(B, L, H)*rs;
    lo = L + dL(B, L, H)*rs;
    h =  H + dH(B, L, H);
}

void GCRecalc::blh_B_to_A(double &br, double &lo, double &h)
{
    double B,L,H;
    B = br; L = lo; H = h;
    N = a / sqrt(1.0 - e2 * sin(B)*sin(B));
    br = B - dB(B, L, H)*rs;
    lo = L - dL(B, L, H)*rs;
    h =  H - dH(B, L, H);
}

void GCRecalc::xyz_A_to_B(double &x_, double &y_, double &z_)
{
    double x = x_, y = y_, z = z_;
    x_ = (1.0 + ms)*( x + wz*rs*y - wy*rs*z) + dx;
    y_ = (1.0 + ms)*(-wz*rs*x + y + wx*rs*z) + dy;
    z_ = (1.0 + ms)*( wy*rs*x - wx*rs*y + z) + dz;
}

void GCRecalc::xyz_B_to_A(double &x_, double &y_, double &z_)
{
    double x = x_, y = y_, z = z_;
    x_ = (1.0 - ms)*( x - wz*rs*y + wy*rs*z) - dx;
    y_ = (1.0 - ms)*( wz*rs*x + y - wx*rs*z) - dy;
    z_ = (1.0 - ms)*(-wy*rs*x + wx*rs*y + z) - dz;
}
