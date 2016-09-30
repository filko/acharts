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
#include "moon_and_sun.hh"

#include <libnova/lunar.h>
#include <libnova/solar.h>

#include "exceptions.hh"

typedef void (* get_equ_coords_t)(double, ln_equ_posn *);
typedef double (* get_sdiam_t)(double);

struct MoonOrSun::Implementation
{
    get_equ_coords_t get_equ_coords;
    get_sdiam_t get_sdiam;

    std::string name;

    Implementation(object_type type)
    {
        switch (type)
        {
            case moon:
                get_equ_coords = &ln_get_lunar_equ_coords;
                get_sdiam = &ln_get_lunar_sdiam;
                name = "Moon";
                break;
            case sun:
                get_equ_coords = &ln_get_solar_equ_coords;
                get_sdiam = &ln_get_solar_sdiam;
                name = "Sun";
                break;
        }
    }
};

MoonOrSun::MoonOrSun(object_type type)
    : imp_(new Implementation(type))
{
}

MoonOrSun::~MoonOrSun()
{
}

const std::string MoonOrSun::name() const
{
    return imp_->name;
}

ln_equ_posn MoonOrSun::get_equ_coords(double JD) const
{
    ln_equ_posn ret;
    imp_->get_equ_coords(JD, &ret);
    return ret;
}

double MoonOrSun::get_sdiam(double JD) const
{
    return imp_->get_sdiam(JD);
}

double MoonOrSun::get_magnitude(double) const
{
    return 0.;
}
