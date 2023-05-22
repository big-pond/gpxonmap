#include "projection.h"
#include <math.h>
#include "util.h"
#include "ellipsoid.h"

/**** Абстрактный класс проекции ****/
Projection::Projection(const QString &el_name, Ellipsoid* el) : ellipsoid(el)
{
    this->el_name = el_name;
    a = ellipsoid->getA();
    e = ellipsoid->getEps();
    k0 =  0.9996;
    b = ellipsoid->getB();
    e2 = e*e;
    e4 = e2*e2;
    e6 = e2*e4;
    e_2 = e*e/(1.0-e*e);
    n = (a-b)/(a+b);
}

//Projection* Projection::createProection(const Projection::Id& _id, Ellipsoid *el)
//{
//    Projection* pro = 0;
//    switch(_id)
//    {
//    case UNKNOWN: pro = new EmptyPro(el); break;
//    case MERCATOR: pro = new MercatorPro(el); break;
//    case GK: pro = new GKPro(el); break;
//    case UTM: pro = new UTMPro(el); break;
//    }
//    return pro;
//}

double Projection::getDistanceOnPlane(const double& x1, const double& y1,
                                     const double& x2,const double& y2)
{
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

/**** Пустая проекция (возвращает координаты без преобразования)****/
EmptyPro::EmptyPro(const QString &el_name, Ellipsoid* el) : Projection(el_name, el)
{
    name = "No transformation";
}

void EmptyPro::geo_to_metre(double &x, double &y, double lat, double lon, bool /*deg*/, int /*zone*/)
{
    x = lat;
    y = lon;
}

void EmptyPro::metre_to_geo(double x, double y, double &lat, double &lon, bool /*deg*/, int /*zone*/)
{
    lat = x;
    lon = y;
}

/**** проекция Меркатора ****/
MercatorPro::MercatorPro(const QString &el_name, Ellipsoid* el) : Projection(el_name, el)
{
    name = "Mercator";
}

void MercatorPro::geo_to_metre(double &x, double &y, double lat, double lon, bool deg, int /*zone*/)
{
    if(deg)
    {
        lat = radians(lat);
        lon = radians(lon);
    }
    y = a*lon;
    x = a*(atanh(sin(lat))-e*atanh(e*sin(lat)));
}

void MercatorPro::metre_to_geo(double x, double y, double &lat, double &lon, bool deg, int /*zone*/)
{
    lon = y/a;
    bool south_hemisphere = x<0;
    double x_ = fabs(x);
    double brn = atan(sinh(x_/a));//  Первое приближение широты
    double brn1 = 0;
    for(;;)
    {
        brn1 = iterBr(x_, brn);
        if(fabs(brn1-brn)<1e-8) break;
        else brn = brn1;
    }
    if(south_hemisphere) lat = -brn1;
    else lat = brn1;
    if(deg)
    {
        lat = degrees(lat);
        lon = degrees(lon);
    }
}

double MercatorPro::iterBr(const double& x, const double& brn)
{
    return asin(1.0 - (1+ sin(brn))*pow(1-e*sin(brn),e)/(exp(2*x/a)*pow(1+e*sin(brn),e)));
}

double MercatorPro::iterLat(const double& y,const int& z,const double& b)
{
    return asin(1.0 - (1+ sin(b))*pow(1-e*sin(b),e)
                /(exp(2.0*M_PI*(1.0-y/pow2f(z-1)))*pow(1+e*sin(b),e)));
}
void MercatorPro::tile_to_degree(const int& x, const int& y, const int& z,
                          double& lat, double& lon)
{
    double  _y;
    lon = x/pow2f(z)*360.0 - 180;
    bool south_hemisphere = y > pow2f(z-1);
    if(south_hemisphere) _y = (double)(y - pow2(z-1)); else _y = (double)y;
    double b1 = atan(sinh(M_PI*(1.0 -_y/pow2f(z-1))));//Первое приближение широты
    double b2 = 0;
    for(;;)
    {
        b2 = iterLat(_y, z, b1);
        if(fabs(b2-b1)<1e-8) break;
        else b1 = b2;
    }
    if(south_hemisphere) lat = -b2;
    else lat = b2;
    lat = degrees(lat);
}

void MercatorPro::degree_to_tile(int& x, int& y, const int& z,
                    const double& lat, const double& lon)
{
    x = (int)(floor((lon + 180.0)/360.0*pow2f(z)));
    double b = radians(lat);
    y = (int)(floor((1.0 - (atanh(sin(b))-e*atanh(e*sin(b)))/M_PI)*pow2f(z-1)));
}

/**** проекция Гаусса-Крюгера ****/
GKPro::GKPro(const QString &el_name, Ellipsoid* el) : Projection(el_name, el)
{
    name = "Gauss-Kruger";
}

void GKPro::geo_to_metre(double &x, double &y, double lat, double lon, bool deg, int zone)
{
    if(deg)
    {
        lat = radians(lat);
        lon = radians(lon);
    }
    double sinB = sin(lat);
    double sin2B = sinB*sinB;
    double sin4B = sin2B*sin2B;
    double sin6B = sin2B*sin4B;
    double loncm = 3.;
    double L = degrees(lon);
    int n = zone;
    if (n==0) n = getZone(L);
    if(n>30) n -= 30;
    else /*if(zone>0&&zone<31)*/ n += 30;

    if(n<31) loncm = 6.*((double)n)-3.;
    else loncm = 6.*((double)n)- 3. - 360.;
    double l = radians(L-loncm);
    double l2 = l*l;
    double sx1 = l2*(109500 - 574700*sin2B + 863700*sin4B - 398600*sin6B);
    double sx2 = l2*(278194 - 830174*sin2B  + 572434*sin4B - 16010*sin6B + sx1);
    double sx3 = l2*(672483.4 - 811219.9*sin2B + 5420*sin4B - 10.6*sin6B + sx2);
    double sx4 = l2*(1594561.25 + 5336.535*sin2B + 26.79*sin4B + 0.149*sin6B + sx3);
    double sy1 = l2*(79690 - 866190*sin2B+ 1730360*sin4B - 945460*sin6B);
    double sy2 = l2*(270806 - 1523417*sin2B + 1327645*sin4B - 21701*sin6B + sy1);
    double sy3 = l2*(1070204.16 - 2136826.66*sin2B + 17.98*sin4B - 11.99*sin6B + sy2);
    double sy4 = l*cos(lat)*(6378245 + 21346.1415*sin2B + 107.159*sin4B + 0.5977*sin6B +sy3);

    x = 6367558.4968*lat - sin(2*lat)*(16002.89 + 66.9607*sin2B + 0.3515*sin4B - sx4);
    y = ((double)((5 + 10*n)*100000)) + sy4;
}

void GKPro::metre_to_geo(double x, double y, double &lat, double &lon, bool deg, int zone)
{
    int n = zone;
    if(n==0)   n = (int)(y*1.0e-6);
    else if(n>30) n -= 30;
    else /*if(zone>0&&zone<31)*/ n += 30;

    double betta = x/6367558.4968;
    double sinbetta = sin(betta);
    double sin2betta = sinbetta*sinbetta;
    double sin4betta = sin2betta*sin2betta;
    double B0 = betta+sin(2*betta)
              * (0.00252588685-0.0000149186*sin2betta+0.00000011904*sin4betta);
    double z0 = (y-(10*n+5)*100000.0)/(6378245*cos(B0));
    double z02 = z0*z0;
    double sinB0 = sin(B0);
    double sin2B0 = sinB0*sinB0;
    double sin4B0 = sin2B0*sin2B0;
    double sin6B0 = sin2B0*sin4B0;
    double sb1 = z02*(0.01672-0.0063*sin2B0+0.01188*sin4B0-0.00328*sin6B0);
    double sb2 = z02*(0.042858-0.025318*sin2B0+0.014346*sin4B0-0.001264*sin6B0-sb1);
    double sb3 = z02*(0.10500614-0.04559916*sin2B0+0.00228901*sin4B0-0.00002987*sin6B0-sb2);
    double DB = -z02*sin(2*B0)*(0.251684631-0.003369263*sin2B0+0.000011276*sin4B0-sb3);
    double sl1=z02*(0.0038+0.0524*sin2B0+0.0482*sin4B0+0.0032*sin6B0);
    double sl2=z02*(0.01225+0.09477*sin2B0+0.03282*sin4B0-0.00034*sin6B0-sl1);
    double sl3=z02*(0.0420025+0.1487407*sin2B0+0.005942*sin4B0-0.000015*sin6B0-sl2);
    double sl4=z02*(0.16778975+0.16273586*sin2B0-0.0005249*sin4B0-0.00000846*sin6B0-sl3);
    double l=z0*(1-0.0033467108*sin2B0-0.0000056002*sin4B0-0.0000000187*sin6B0-sl4);
    lat = B0+DB;
    double loncm = 3.;
    if(n<31) loncm = 6.*((double)n)-3.;
    else loncm = 6.*((double)n)- 3. - 360.;
    lon = radians(loncm) + l; //  lon = 6*(n-0.5)/57.29577951+l;
    if(deg)
    {
        lat = degrees(lat);
        lon = degrees(lon);
    }}

/**** проекция UTM ****/
UTMPro::UTMPro(const QString &el_name, Ellipsoid* el) : Projection(el_name, el)
{
    name = "UTM";
}

void UTMPro::geo_to_metre(double &x, double &y, double lat, double lon, bool deg, int zone)
{
    if(deg)
    {
        lat = radians(lat);
        lon = radians(lon);
    }
    double londeg = degrees(lon);  //долгота в градусах
    int m = zone;
    if(m==0)
        m = 31 + (int)(londeg/6.);
    double lon0 =  radians(6.*((double)m)-183.);
    double sin2lat = sin(lat)*sin(lat);
    double nu = a/pow(1.0-e*e*sin2lat, 0.5);
    double p = lon-lon0;
    double A = a*(1. - n + 5./4.*(n*n - n*n*n) + 81./64.*(n*n*n*n-n*n*n*n*n));
    double B = 3./2.*a*(n - n*n + 7./8.*(n*n*n - n*n*n*n) + 55./64.*n*n*n*n*n);
    double C = 15./16.*a*(n*n - n*n*n + 3./4.*(n*n*n*n - n*n*n*n*n));
    double D = 35./48.*a*(n*n*n - n*n*n*n + 11./16.*n*n*n*n*n);
    double E = 315./512.*a*(n*n*n*n - n*n*n*n*n);
    double S = A*lat - B*sin(2.*lat) + C*sin(4.*lat) - D*sin(6.*lat) + E*sin(8.*lat);
    double coslt = cos(lat);
    double cos2lt = coslt*coslt;
    double cos3lt = coslt*cos2lt;
    double cos4lt = cos2lt*cos2lt;
    double cos5lt = cos2lt*cos3lt;
    double cos6lt = cos2lt*cos4lt;
    double cos7lt = cos3lt*cos4lt;
    double cos8lt = cos4lt*cos4lt;
    double sinlt = sin(lat);
    double e_4 = e_2*e_2;
    double e_6 = e_2*e_2*e_2;
    double e_8 = e_4*e_4;
    double tanlt = tan(lat);
    double tan2lt = tanlt*tanlt;
    double tan4lt = tan2lt*tan2lt;
    double tan6lt = tan2lt*tan4lt;
    double T1 = S*k0;
    double T2 = nu*sinlt*cos(lat)*k0*0.5;
    double T3 = nu*sinlt*cos3lt*k0/24.*(5. - tan2lt + 9.*e_2*cos2lt + 4*e_4*cos4lt);
    double T4 = nu*sinlt*cos5lt*k0/720.*(61. - 58.*tan2lt + tan4lt
              + 270.*e_2*cos2lt - 330.*tan2lt*e_2*cos2lt
              + 445.*e_4*cos4lt + 324.*e_6*cos6lt - 680.*tan2lt*e_4*cos4lt
              + 88.*e_8*cos8lt - 600.*tan2lt*e_6*cos6lt - 192.*tan2lt*e_8*cos8lt);
    double T5 = nu*sinlt*cos7lt*k0/40320.*(1385. - 3111.*tan2lt + 543.*tan4lt - tan6lt);
    double T6 = nu*cos(lat)*k0;
    double T7 = nu*cos3lt*k0/6.*(1. - tan2lt + e_2*cos2lt);
    double T8 = nu*cos5lt*k0/120*(5. - 18.*tan2lt + tan4lt + 14.*e_2*cos2lt
              - 58.*tan2lt*e_2*cos2lt + 13.*e_4*cos4lt
              + 4.*e_6*cos6lt - 64.*tan2lt*e_4*cos4lt - 24.*tan2lt*e_6*cos6lt);
    double T9 = nu*cos7lt*k0/5040.*(61. - 479.*tan2lt + 179.*tan4lt - tan6lt);

    y = T1 + p*p*T2 + T3*p*p*p*p + T4*p*p*p*p*p*p +T5*p*p*p*p*p*p*p*p;

    x = T6*p + T7*p*p*p + T8*p*p*p*p*p + T9*p*p*p*p*p*p*p;
    x += 500000.;
    x += ((double)m)*1.e6;}

void UTMPro::metre_to_geo(double x, double y, double &lat, double &lon, bool deg, int zone)
{
    int m = zone;
    if(zone==0)
      m = (int)(x*1.0e-6);
    double M = y/k0;
    double mu = M/(a*(1.- e2*0.25 - 3.*e4/64. - 5.*e6/256.));
    double pow1e2 = pow(1. - e2, 0.5);
    double e1 = (1. - pow1e2)/(1.+ pow1e2);
    double J1 = (3.*e1*0.5 - 27.*e1*e1*e1/32.);
    double J2 = (21.*e1*e1/16. - 55.*e1*e1*e1*e1/32.);
    double J3 = (151.*e1*e1*e1/96.);
    double J4 = (1097.*e1*e1*e1*e1/512.);

    double fp = mu + J1*sin(2.*mu) + J2*sin(4.*mu) + J3*sin(6.*mu) + J4*sin(8.*mu);
    double C1 = e_2*cos(fp)*cos(fp);
    double T1 = tan(fp)*tan(fp);
    double R1 = a*(1. - e2)/pow(1. - e2*sin(fp)*sin(fp), 1.5);
    double N1 = a/pow(1. - e2*sin(fp)*sin(fp), 0.5);
    double xloc = x - m*1.e6;
    double D = (500000. - xloc)/(N1*k0);


    double Q1 = N1*tan(fp)/R1;
    double Q2 = 0.5*D*D;
    double Q3 = (5. + 3.*T1 + 10.*C1 - 4.*C1*C1 - 9.*e_2)*D*D*D*D/24.;
    double Q4 = (61. + 90.*T1 + 298.*C1 + 45.*T1*T1 - 3.*C1*C1 - 252.*e_2)*D*D*D*D*D*D/720.;
    lat = fp - Q1*(Q2 - Q3 + Q4);
    double Q6 = (1. + 2.*T1 + C1)*D*D*D/6.;
    double Q7 = (5. - 2.*C1 + 28.*T1 - 3.*C1*C1 + 8.*e_2 + 24.*T1*T1)*D*D*D*D*D/120.;

    double lon0 = radians(6.*((double)m)-183.);
    lon = lon0 - (D - Q6 + Q7)/cos(fp);
    if(deg)
    {
        lat = degrees(lat);
        lon = degrees(lon);
    }
}
