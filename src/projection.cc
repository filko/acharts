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
#include "projection.hh"

#include <boost/algorithm/string.hpp>
#include <array>
#include <cmath>
#include <libnova/precession.h>
#include <libnova/utility.h>
#include <stdexcept>

#include "exceptions.hh"

namespace {

template <typename T, int D>
struct vec
{
    std::array<T, D> data;
    T & operator[](std::size_t index) { return data[index]; }
    T operator[](std::size_t index) const { return data[index]; }

};

template <typename T, int D>
vec<T, D> operator*(T l, const vec<T,D> & r)
{
    vec<T, D> ret;
    for (int i(0); i < D; ++i)
        ret[i] = l * r[i];
    return ret;
}

template <typename T, int D>
vec<T, D> operator+(vec<T, D> l, const vec<T, D> & r)
{
    for (int i(0); i < D; ++i)
        l[i] += r[i];
    return l;
}

template <typename T, int D>
T operator*(const vec<T, D> & l, const vec<T, D> & r)
{
    T ret = {};
    for (int i(0); i < D; ++i)
        ret += l[i] * r[i];
    return ret;
}

template <typename T>
vec<T, 3> cross(const vec<T, 3> & l, const vec<T, 3> & r)
{
    return vec<T, 3>{{{
                l[1]*r[2] - r[1]*l[2],
                r[0]*l[2] - l[0]*r[2],
                l[0]*r[1] - r[0]*l[1]
                    }}};
}

typedef vec<long double, 3> vec3;

}

SphericalCoord::SphericalCoord(const ln_equ_posn & rh)
    : ra(ln_deg_to_rad(rh.ra)),
      dec(ln_deg_to_rad(rh.dec))
{
}

SphericalCoord::SphericalCoord(double r, double d)
    : ra(r), dec(d)
{
}

Projection::Projection(const CanvasPoint & canvas, const ln_equ_posn & apparent_canvas, const ln_equ_posn & center)
    : canvas_(canvas), apparent_canvas_(apparent_canvas), center_(center),
      rotationSin_(0.), rotationCos_(1.)
{
    if (0. == apparent_canvas_.ra)
    {
        if (0. == apparent_canvas_.dec)
            throw std::runtime_error("Need to specify at least one apparent size dimention non-zero!");

        apparent_canvas_.ra = apparent_canvas_.dec * canvas_.x / canvas_.y;
    }
    else if (0. == apparent_canvas_.dec)
    {
        apparent_canvas_.dec = apparent_canvas_.ra * canvas_.y / canvas_.x;
    }

    // negate x, because for svg up is down
    // negate y, because right ascention is going left to right
    scaleX_ = -canvas_.x / apparent_canvas_.ra;
    scaleY_ = -canvas_.y / apparent_canvas_.dec;
}

Projection::~Projection()
{
}

CanvasPoint Projection::project(const ln_equ_posn & pos) const
{
    return project_imp(SphericalCoord{pos});
}

double Projection::scale_at_point(const ln_equ_posn & pos) const
{
    ln_equ_posn p2(pos);
    p2.ra += 0.1;
    CanvasPoint o1(this->project(pos)), o2(this->project(p2));
    return (o1.x - o2.x) * 10.;
}

void Projection::rotate_to_level(const ln_equ_posn & beg, const ln_equ_posn & end)
{
    CanvasPoint center(project(beg)),
        leveling(project(end));

    CanvasPoint diff(center - leveling);
    double angle(M_PI - std::atan2(diff.y, diff.x));
    rotationSin_ = std::sin(angle);
    rotationCos_ = std::cos(angle);

    this->rotate_to_level_imp();
}

void Projection::rotate_to_level_imp()
{ }

double Projection::max_distance() const
{
    return canvas_.x / 4.;
}

class AzimuthalEquidistantProjection
    : public Projection
{
public:
    AzimuthalEquidistantProjection(const CanvasPoint & canvas, const ln_equ_posn & apparent_canvas,
                                   const ln_equ_posn & center)
        : Projection(canvas, apparent_canvas, center)
    {
    }

    virtual CanvasPoint project_imp(const SphericalCoord & pos) const
    {
        double cosc(sin(center_.dec) * sin(pos.dec) + cos(center_.dec) * cos(pos.dec) * cos(pos.ra - center_.ra));
        double c(acos(cosc));
        if (fabs(c - M_PI) < 0.0001)
            return CanvasPoint{NAN, NAN};

        double k;
        if (c == 0)
            k = 1;
        else
            k = c / sin(c);
        double x(k * cos(pos.dec) * sin(pos.ra - center_.ra));
        double y(k * (cos(center_.dec) * sin(pos.dec) - sin(center_.dec) * cos(pos.dec) * cos(pos.ra - center_.ra)));

        return CanvasPoint(scaleX_ * x, scaleY_ * y)
            .rotate(rotationSin_, rotationCos_);
    }
};

class CylindricalEquidistantProjection
    : public Projection
{
    vec3 rotation_axis_;
public:
    CylindricalEquidistantProjection(const CanvasPoint & canvas, const ln_equ_posn & apparent_canvas,
                                     const ln_equ_posn & center)
        : Projection(canvas, apparent_canvas, center),
          rotation_axis_{{{ 1., 0., 0. }}}
    { }

    virtual CanvasPoint project_imp(const SphericalCoord & pos) const
    {
        SphericalCoord rotated = rotate(pos);
        double x(rotated.ra - center_.ra);
        if (x > M_PI)
            x -= 2 * M_PI;
        else if (x < -M_PI)
            x += 2 * M_PI;
        double y(rotated.dec - center_.dec);

        return CanvasPoint(scaleX_ * x, scaleY_ * y);
    }

    virtual void rotate_to_level_imp()
    {
        SphericalCoord axis(center_);
        axis.dec = M_PI_2 - axis.dec;
        rotation_axis_ = {{{sin(axis.dec) * cos(axis.ra), sin(axis.dec) * sin(axis.ra), cos(axis.dec)}}};
    }

private:
    SphericalCoord rotate(SphericalCoord pos) const
    {
        pos.dec = M_PI_2 - pos.dec;

        vec3 a = {{{sin(pos.dec) * cos(pos.ra),   sin(pos.dec) * sin(pos.ra),   cos(pos.dec)}}};
        vec3 b = rotationCos_ * a + rotationSin_ * cross(rotation_axis_, a) + (rotation_axis_ * a) * (1. - rotationCos_) * rotation_axis_;

        return SphericalCoord(double(std::atan2(b[1], b[0])),
                              double(M_PI_2 - std::atan2(std::hypot(b[0], b[1]), b[2])));
    }
};

std::shared_ptr<Projection> ProjectionFactory::create(const std::string & type,
                                                      const CanvasPoint & canvas, const ln_equ_posn & apparent_canvas,
                                                      const ln_equ_posn & center)
{
    std::string t(boost::algorithm::to_lower_copy(type));

    if ("azimuthalequidistant" == t)
        return std::make_shared<AzimuthalEquidistantProjection>(canvas, apparent_canvas, center);
    if ("cylindricalequidistant" == t)
        return std::make_shared<CylindricalEquidistantProjection>(canvas, apparent_canvas, center);

    throw ConfigError("Unknown projection specified: " + type);
}

ln_equ_posn convert_epoch(ln_equ_posn in, double fromJD, double toJD)
{
    if (fromJD == toJD)
        return in;
    ln_equ_posn out;
    ln_get_equ_prec2(&in, fromJD, toJD, &out);
    return out;
}
