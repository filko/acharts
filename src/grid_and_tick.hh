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
#ifndef ACHARTS_GRID_AND_TICK_HH
#define ACHARTS_GRID_AND_TICK_HH 1

#include "types.hh"

enum class Coordinates
{
    Equatorial,
    Horizontal,
    Ecliptic
};

enum class Plane
{
    Parallel,
    Meridian
};

struct Grid
{
    Coordinates coordinates;
    Plane plane;
    std::string name;
    angle start, end, step = {1}, density = {2};
};

struct Tick
{
    Coordinates coordinates;
    Plane plane;
    std::string name;
    enum Type { as_degrees, as_hours } display = as_degrees;
    angle start, end, step = {1}, base = {0};
};

#endif
