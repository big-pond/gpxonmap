#include "waypoint.h"

WayPoint::WayPoint(double lt, double ln, double at, QDateTime tm)
    : TrackPoint(lt, ln, at, tm), _name(), _cmt(), _desc(), _src(), _sym(), _type(),
      _image(0), _dx(0), _dy(0), _marking(), _fuel(0), _namepos(BottomRight), _control(false), _arr(tm), _dep(tm)
{}

WayPoint::WayPoint(double lt, double ln, double at, QDate dt, QTime tm)
    : TrackPoint(lt, ln, at, QDateTime(dt, tm)), _name(), _cmt(), _desc(), _src(), _sym(), _type(),
      _image(0), _dx(0), _dy(0), _marking(), _fuel(0), _namepos(BottomRight), _control(false), _arr(dt,tm), _dep(dt,tm)
{}

WayPoint::WayPoint(const WayPoint &other)
    : TrackPoint(other), _name(other._name), _cmt(other._cmt), _desc(other._desc),
      _src(other._src), _sym(other._sym), _type(other._type),
      _image(other._image), _dx(other._dx), _dy(other._dy),
      _marking(other._marking), _fuel(other._fuel)  , _namepos(other._namepos),
      _control(other._control), _arr(other._arr), _dep(other._dep)
{}

WayPoint&  WayPoint::operator=(const WayPoint &other)
{
    TrackPoint::operator=(other);
    _name = other._name;
    _cmt = other._cmt;
    _desc = other._desc;
    _src = other._src;
    _sym = other._sym;
    _type = other._type;

    _image = other._image;
    _dx = other._dx;
    _dy = other._dy;

    _marking = other._marking;
    _fuel = other._fuel;
    _namepos = other._namepos;
    _control = other._control;
    _arr = other._arr;
    _dep = other._dep;
    return *this;
}

bool WayPoint::operator<( const WayPoint &other ) const
{
    return TrackPoint::operator<(other);
}

