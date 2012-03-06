#ifndef ACHART_CANVAS_HH
#define ACHART_CANVAS_HH 1

#include <cmath>

struct CanvasPoint
{
    double x, y;

    CanvasPoint()
        : x(0), y(0)
    {
    }

    CanvasPoint(double x, double y)
        : x(x), y(y)
    {
    }

    bool nan() const
    {
        return isnan(x);
    }

    CanvasPoint & operator+=(const CanvasPoint & rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    CanvasPoint & operator-=(const CanvasPoint & rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    CanvasPoint & operator/=(double s)
    {
        x /= s;
        y /= s;
        return *this;
    }

    CanvasPoint & operator*=(double s)
    {
        x *= s;
        y *= s;
        return *this;
    }

    double norm() const
    {
        return sqrt(x*x + y*y);
    }
};

inline CanvasPoint operator+(const CanvasPoint & l, const CanvasPoint & r)
{
    CanvasPoint ret(l);
    ret += r;
    return ret;
}

inline CanvasPoint operator-(const CanvasPoint & l, const CanvasPoint & r)
{
    CanvasPoint ret(l);
    ret -= r;
    return ret;
}

inline CanvasPoint operator/(const CanvasPoint & l, double s)
{
    CanvasPoint ret(l);
    ret /= s;
    return ret;
}

inline CanvasPoint operator*(double s, const CanvasPoint & l)
{
    CanvasPoint ret(l);
    ret *= s;
    return ret;
}

inline CanvasPoint operator*(const CanvasPoint & l, double s)
{
    CanvasPoint ret(l);
    ret *= s;
    return ret;
}

inline CanvasPoint operator-(const CanvasPoint & r)
{
    CanvasPoint ret{-r.x, -r.y};
    return ret;
}

#endif
