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
#ifndef GRAPH_PROJECTION_HH
#define GRAPH_PROJECTION_HH 1

#include <libnova/ln_types.h>
#include <memory>

#include "canvas.hh"

struct SphericalCoord
{
    double ra, dec;

    SphericalCoord(const ln_equ_posn & rh);
    SphericalCoord(double r, double d);
};

class Projection
{
    Projection(const Projection &) = delete;
    Projection & operator=(const Projection &) = delete;

protected:
    Projection(const CanvasPoint & canvas, const ln_equ_posn & apparent_canvas, const ln_equ_posn & center);

public:
    virtual ~Projection();
    CanvasPoint project(const ln_equ_posn & pos) const;
    virtual double scale_at_point(const ln_equ_posn & pos) const;
    void rotate_to_level(const ln_equ_posn & beg, const ln_equ_posn & end);
    double max_distance() const;

protected:
    virtual CanvasPoint project_imp(const SphericalCoord & pos) const = 0;
    virtual void rotate_to_level_imp();

    const CanvasPoint canvas_;
    SphericalCoord apparent_canvas_;
    const SphericalCoord center_;
    double scaleX_, scaleY_;
    long double rotationSin_, rotationCos_;
};

class ProjectionFactory
{
public:
    static std::shared_ptr<Projection> create(const std::string & type,
                                              const CanvasPoint & canvas, const ln_equ_posn & apparent_canvas,
                                              const ln_equ_posn & center);
};

ln_equ_posn convert_epoch(ln_equ_posn in, double fromJD, double toJD);

#endif
