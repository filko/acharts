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
#ifndef CHARTS_PLANET_HH
#define CHARTS_PLANET_HH 1

#include "solar_object.hh"

#include <libnova/ln_types.h>
#include <memory>

class Planet
    : public SolarObject
{
    struct Implementation;
    std::unique_ptr<Implementation> imp_;

public:
    explicit Planet(const std::string & name);
    virtual ~Planet();

    virtual const std::string name() const;

    virtual ln_equ_posn get_equ_coords(double JD) const;
    virtual double get_magnitude(double JD) const;
    virtual double get_sdiam(double JD) const;
};

#endif
