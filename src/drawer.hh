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
#ifndef GRAPH_DRAWER_HH
#define GRAPH_DRAWER_HH 1

#include <deque>
#include <memory>

#include "bezier.hh"
#include "canvas.hh"
#include "projection.hh"
#include "solar_object.hh"
#include "track.hh"

const std::vector<CanvasPoint> create_path_from_track(const std::shared_ptr<Projection> & projection,
                                                      const Track & track, const double epoch,
                                                      const std::shared_ptr<const SolarObject> & object);

const std::deque<BezierCurve> create_bezier_from_path(const std::vector<CanvasPoint> & path, double max_distance);
const std::deque<BezierCurve> create_bezier_from_path(const std::shared_ptr<Projection> & projection, const std::vector<ln_equ_posn> & path);

#endif
