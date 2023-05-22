//Copyright (C) Yuriy Popov 2012
//Contact: Yuriy Popov (urmingard@yandex.ru)
//This file is part of application TripOnMap

#include <math.h>
#include "ellipsoid.h"
#include "util.h"
//#include <QDebug>

//***********************************************************************

Ellipsoid::Ellipsoid()
{
    setData(6378137.0, 1.0/298.257223563); //WGS-84
}

// _a - большая полуось, p -  полярное сжатие или малая полуось
Ellipsoid::Ellipsoid(const double& _a, const double& p)
{
    setData( _a, p);
}

void Ellipsoid::setData(const double& _a, const double &p)
{
    a = _a;
    if(p<1)
    {
        alpha = p;
        b = a - a*alpha;
    }
    else
    {
        b = p;
        alpha =  (a-b)/a;
    }
    eps = sqrt(a*a-b*b)/a;
    eps2 = 2.0 * alpha - alpha*alpha;   //или  = (a*a-b*b)/(a*a);
    eps_2 = (a*a-b*b)/(b*b);
    eps_ = sqrt(eps_2);
}

double Ellipsoid::N(const double& B)
{
    return a/sqrt(1.0-eps*eps*sin(B)*sin(B));
}

double Ellipsoid::M(const double& B)
{
    return a*(1-eps*eps)/pow(1.0-eps*eps*sin(B)*sin(B),3.0/2.0);
}

double Ellipsoid:: mR(const double& B)
{
    return a*sqrt(1.0-eps2)/(1.0 - eps2*sin(B)*sin(B));
}

void Ellipsoid::blhrad_to_xyz(double& x, double& y, double& z,
                        const double& br,  const double& lo, const double& h)
{ //по ГОСТ 51794-2001
    double n = N(br);
    x = (n + h)*cos(br)*cos(lo);
    y = (n + h)*cos(br)*sin(lo);
    z = ((1.0 - eps*eps)*n + h)*sin(br);
}

void Ellipsoid::xyz_to_blhrad(const double& x, const double& y, const double& z,
                           double& br,  double& lo, double& h)
{//по ГОСТ 51794-2008
    double D = sqrt(x*x + y*y);
    if(D==0)
    {
        br = M_PI_2*z/fabs(z);
        lo = 0;
        h = z*sin(br)-a*sqrt(1.0-eps*eps*sin(br)*sin(br));
    }
    else if (D>0)
    {
        double la = fabs(asin(y/D));
        if(y<0&&x>0) lo = 2*M_PI - la;
        else if (y<0&&x<0) lo = M_PI + la;
        else if (y>0&&x<0) lo = M_PI - la;
        else if (y>0&&x>0) lo = la;
        else if (y==0&&x>0) lo = 0;
        else if (y==0&&x<0) lo = M_PI;
        if(z==0)
        {
            br = 0;
            h = D - a;
        }
        else
        {
            double r = sqrt(x*x + y*y + z*z);
            double c = asin(z/r);
            double p = eps*eps*a/(2*r);
            double s1,s2 = 0;
            do
            {
                s1 = s2;
                br = c + s1;
                s2 = asin(p*sin(2*br)/sqrt(1.0-eps*eps*sin(br)*sin(br)));
            }
            while(fabs(s2-s1)>4.84814E-09);
           //При преобразованиях координат в качестве допуска прекращения
           //итеративного процесса принимают значение 0,0001". В этом случае
           //погрешность вычисления геодезической высоты не превышает 0,003 м.
           //0,0001" fabs(s2-s1)=4,84814E-10
           //0,001"  fabs(s2-s1)=4,84814E-09
           //0,01"   fabs(s2-s1)=4,84814E-08
            h = D*cos(br) + z*sin(br) - a*sqrt(1.0-eps*eps*sin(br)*sin(br));
        }
    }
}

void Ellipsoid::blhdeg_to_xyz(double& x, double& y, double& z,
             const double& br,  const double& lo, const double& h)
{
    blhrad_to_xyz(x, y, z, radians(br), radians(lo),h);
}

void Ellipsoid::xyz_to_blhdeg(const double& x, const double& y, const double& z,
                double& br,  double& lo, double& h)
{
    xyz_to_blhrad(x, y, z, br, lo,  h);
    br = degrees(br);
    lo = degrees(lo);
}

double Ellipsoid::getDistanceLn(const double& b1, const double& l1, const double& h1,
                                const double& b2, const double& l2, const double& h2)
{
    double x1, y1, z1, x2, y2, z2;
    blhdeg_to_xyz(x1, y1, z1, b1, l1, h1);
    blhdeg_to_xyz(x2, y2, z2, b2, l2, h2);
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
}

double Ellipsoid::getDistanceEl(const double& b1, const double& l1,
                                const double& b2, const double& l2)
{
    double dist = 0;
    double x1, y1, z1, x2, y2, z2;
    double hord, alpha;
    if(a>b)////для эллипсоида
    {
        //по сфере среднего радиуса кривизны
        blhdeg_to_xyz(x1, y1, z1, b1, l1, 0.0);
        blhdeg_to_xyz(x2, y2, z2, b2, l2, 0.0);
        hord = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
        double r = mR(0.5*(radians(b1)+radians(b2)));
        alpha = 2.0*asin(hord/(2.0*r));
        dist = r*alpha;
    }
    else // для сферы
    {
        blhdeg_to_xyz(x1, y1, z1, b1, l1, 0.0);
        blhdeg_to_xyz(x2, y2, z2, b2, l2, 0.0);
        hord = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
        alpha = 2.0*asin(hord/(2.0*a));
        dist = a*alpha;
    }
    return dist;
}

double Ellipsoid::getEquatorLength()
{
    return 2.0*M_PI*a;
}

double Ellipsoid::getParalleLengthOnLatRad(const double &lat)
{
    return 2.0*M_PI*a*cos(lat)/sqrt(1.0-eps*eps*sin(lat)*sin(lat));
}

double Ellipsoid::getParalleLengthOnLatDeg(const double &lat)
{
    double B = radians(lat);
    return getParalleLengthOnLatRad(B);
}

double Ellipsoid::getDistanceV(const double& b1, const double& l1,
                               const double& b2, const double& l2,
                               double& az1, double& az2)
{
    double f = alpha;
    double L = radians(l2) - radians(l1);
    double lambda = L;
    double U1 = atan((1. - f)*tan(radians(b1)));
    double U2 = atan((1. - f)*tan(radians(b2)));
    double cosU1 = cos(U1);
    double cosU2 = cos(U2);
    double sinU1 = sin(U1);
    double sinU2 = sin(U2);
    double cos2alpha, sigma, sinsigma, cossigma, cossigmam2;
    for(;;)
    {
        double sinlambda = sin(lambda);
        double coslambda = cos(lambda);
        sinsigma = sqrt(cosU2*sinlambda*cosU2*sinlambda
                               + pow(cosU1*sinU2 - sinU1*cosU2*coslambda, 2.));
        cossigma = sinU1*sinU2 + cosU1*cosU2*coslambda;
        sigma = atan2(sinsigma, cossigma);
        double sinalpha = (sinsigma==0.) ? 0. : (cosU1*cosU2*sinlambda)/sinsigma;

        cos2alpha = 1. - sinalpha*sinalpha;
        cossigmam2 = (cos2alpha==0.) ? 0. : cossigma - 2.*sinU1*sinU2/cos2alpha;
        double C = f/16.*cos2alpha*(4. + f*(4. - 3.*cos2alpha));
        double lambda1 = L + (1. - C)*f*sinalpha*(sigma + C*sinsigma*(cossigmam2 + C*cossigma
                                                *(-1. + 2*cossigmam2*cossigmam2)));
        if(fabs(lambda-lambda1)<1.e-12) break;
        else lambda = lambda1;
    }
    double u2 = cos2alpha*(a*a - b*b)/(b*b);
    double A = 1. + u2/16384.*(4096. + u2*(-768. + u2*(320. - 175.*u2)));
    double B = u2/1024.*(256. + u2*(-128. + u2*(74. - 47.*u2)));
    double dsigma = B*sinsigma*(cossigmam2 + 0.25*B*(cossigma*(-1. + 2.*cossigmam2*cossigmam2)
                              - (B/6.)*cossigmam2*(-3. + 4*sinsigma*sinsigma)
                                      *(-3. + 4.*cossigmam2*cossigmam2)));
    double azrad = atan2(cosU2*sin(lambda), (cosU1*sinU2 - sinU1*cosU2*cos(lambda)));
    if(azrad<0.) azrad += M_PI*M_PI;
    az1 = degrees(azrad);
    if (az1 >= 360.) az1 -= 360.;
    azrad = atan2(cosU1*sin(lambda), (-sinU1*cosU2 + cosU1*sinU2*cos(lambda))) + M_PI;
    if(azrad<0.) azrad += M_PI*M_PI;
    az2 = degrees(azrad);
    if (az2 >= 360.) az2 -= 360.;
    return b*A*(sigma - dsigma);
}

double Ellipsoid::getDistanceV(const double& b1, const double& l1,
                               const double& b2, const double& l2)
{
    double f = alpha;
    double L = radians(l2) - radians(l1);
    double lambda = L;
    double U1 = atan((1. - f)*tan(radians(b1)));
    double U2 = atan((1. - f)*tan(radians(b2)));
    double cosU1 = cos(U1);
    double cosU2 = cos(U2);
    double sinU1 = sin(U1);
    double sinU2 = sin(U2);
    double cos2alpha, sigma, sinsigma, cossigma, cossigmam2;
    for(;;)
    {
        double sinlambda = sin(lambda);
        double coslambda = cos(lambda);
        sinsigma = sqrt(cosU2*sinlambda*cosU2*sinlambda
                               + pow(cosU1*sinU2 - sinU1*cosU2*coslambda, 2.));
        cossigma = sinU1*sinU2 + cosU1*cosU2*coslambda;
        sigma = atan2(sinsigma, cossigma);
        double sinalpha = (sinsigma==0.) ? 0. : (cosU1*cosU2*sinlambda)/sinsigma;

        cos2alpha = 1. - sinalpha*sinalpha;
        cossigmam2 = (cos2alpha==0.) ? 0. : cossigma - 2.*sinU1*sinU2/cos2alpha;
        double C = f/16.*cos2alpha*(4. + f*(4. - 3.*cos2alpha));
        double lambda1 = L + (1. - C)*f*sinalpha*(sigma + C*sinsigma*(cossigmam2 + C*cossigma
                                                *(-1. + 2*cossigmam2*cossigmam2)));
        if(fabs(lambda-lambda1)<1.e-12) break;
        else lambda = lambda1;
    }
    double u2 = cos2alpha*(a*a - b*b)/(b*b);
    double A = 1. + u2/16384.*(4096. + u2*(-768. + u2*(320. - 175.*u2)));
    double B = u2/1024.*(256. + u2*(-128. + u2*(74. - 47.*u2)));
    double dsigma = B*sinsigma*(cossigmam2 + 0.25*B*(cossigma*(-1. + 2.*cossigmam2*cossigmam2)
                              - (B/6.)*cossigmam2*(-3. + 4*sinsigma*sinsigma)
                                      *(-3. + 4.*cossigmam2*cossigmam2)));
    return b*A*(sigma - dsigma);
}
double Ellipsoid::getDistanceV1(const double& b1, const double& l1, const double& h1,
                                const double& b2, const double& l2, const double& h2)
{
    double f = alpha;
    double L = radians(l2) - radians(l1);
    double lambda = L;
    double U1 = atan((1. - f)*tan(radians(b1)));
    double U2 = atan((1. - f)*tan(radians(b2)));
    double cosU1 = cos(U1);
    double cosU2 = cos(U2);
    double sinU1 = sin(U1);
    double sinU2 = sin(U2);
    double cos2alpha, sigma, sinsigma, cossigma, cossigmam2;
    for(;;)
    {
        double sinlambda = sin(lambda);
        double coslambda = cos(lambda);
        sinsigma = sqrt(cosU2*sinlambda*cosU2*sinlambda
                               + pow(cosU1*sinU2 - sinU1*cosU2*coslambda, 2.));
        cossigma = sinU1*sinU2 + cosU1*cosU2*coslambda;
        sigma = atan2(sinsigma, cossigma);
        double sinalpha = (sinsigma==0.) ? 0. : (cosU1*cosU2*sinlambda)/sinsigma;

        cos2alpha = 1. - sinalpha*sinalpha;
        cossigmam2 = (cos2alpha==0.) ? 0. : cossigma - 2.*sinU1*sinU2/cos2alpha;
        double C = f/16.*cos2alpha*(4. + f*(4. - 3.*cos2alpha));
        double lambda1 = L + (1. - C)*f*sinalpha*(sigma + C*sinsigma*(cossigmam2 + C*cossigma
                                                *(-1. + 2*cossigmam2*cossigmam2)));
        if(fabs(lambda-lambda1)<1.e-12) break;
        else lambda = lambda1;
    }
    double h = (h1+h2)*0.5;
    double a_ = a+h;
    double b_ = b+h;
    double u2 = cos2alpha*(a_*a_ - b_*b_)/(b_*b_);
    double A = 1. + u2/16384.*(4096. + u2*(-768. + u2*(320. - 175.*u2)));
    double B = u2/1024.*(256. + u2*(-128. + u2*(74. - 47.*u2)));
    double dsigma = B*sinsigma*(cossigmam2 + 0.25*B*(cossigma*(-1. + 2.*cossigmam2*cossigmam2)
                              - (B/6.)*cossigmam2*(-3. + 4*sinsigma*sinsigma)
                                      *(-3. + 4.*cossigmam2*cossigmam2)));
    return b_*A*(sigma - dsigma);
}
void Ellipsoid::getPointV(const double& b1, const double& l1,const double& s,
                          const double& az1, double& b2, double& l2, double& az2)
{
    double f = alpha;
    double azr = radians(az1);
    double tanU1 = (1. - f)*tan(radians(b1));
    double U1 = atan(tanU1);
    double sinU1 = sin(U1);
    double cosU1 = cos(U1);
    double sigma1 = atan(tanU1/cos(azr));
    double sinalpha = cosU1*sin(azr);
    double cos2alpha = (1. - sinalpha)*(1. + sinalpha);
    double u2 = cos2alpha*(a*a - b*b)/(b*b);
    double A = 1. + u2/16384.*(4096. + u2*(-768. + u2*(320. - 175.*u2)));
    double B = u2/1024.*(256. + u2*(-128. + u2*(74. - 47.*u2)));
    double sigma = s/(b*A);
    double sigmam2, cossigmam2, sinsigma, cossigma;
    for(;;)
    {
        sigmam2 = 2.*sigma1 + sigma;
        sinsigma = sin(sigma);
        cossigma = cos(sigma);
        cossigmam2 = cos(sigmam2);
        double dsigma = B*sinsigma*(cossigmam2 + 0.25*B*(cossigma*(-1. + 2.*cossigmam2*cossigmam2)
                                    - (B/6.)*cossigmam2*(-3. + 4*sinsigma*sinsigma)
                                            *(-3. + 4.*cossigmam2*cossigmam2)));
        double sigmanew = s/(b*A) + dsigma;
        if(fabs(sigma-sigmanew)<1.e-12) break;
        else sigma = sigmanew;
    }
    double b2rad = atan2(sinU1*cossigma + cosU1*sinsigma*cos(azr), (1. - f)
               *sqrt(sinalpha*sinalpha + pow(sinU1*sinsigma - cosU1*cossigma*cos(azr),2.)));
    double lambda = atan2(sinsigma*sin(azr), (cosU1*cossigma - sinU1*sinsigma*cos(azr)));
    double C = f/16.*cos2alpha*(4. + f*(4. - 3.*cos2alpha));
    double L = lambda - (1. - C)*f*sinalpha*(sigma + C*sinsigma*(cossigmam2 + C*cossigma
                                            *(-1. + 2*cossigmam2*cossigmam2)));
    double azr2 = atan2(sinalpha, (-sinU1*sinsigma + cosU1*cossigma*cos(azr))) + M_PI;
    az2 = degrees(azr2);
    b2 = degrees(b2rad);
    l2 = l1 + degrees(L);
}
