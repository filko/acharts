/*
 * Copyright (c) 2012-2016 Łukasz P. Michalik <lpmichalik@googlemail.com>

 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:

 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
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
        return std::isnan(x);
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

    const CanvasPoint rotate(double sin, double cos) const
    {
        return CanvasPoint(x * cos - y * sin, x * sin + y * cos);
    }

    const CanvasPoint rotate(double radians) const
    {
        return rotate(std::sin(radians), std::cos(radians));
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

inline double operator*(const CanvasPoint & l, const CanvasPoint & r)
{
    return l.x * r.x + l.y * r.y;
}

#endif
