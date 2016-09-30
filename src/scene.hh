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
#ifndef ACHARTS_SCENE_STORAGE_HH_
#define ACHARTS_SCENE_STORAGE_HH_ 1

#include <boost/variant/recursive_variant.hpp>
#include <deque>
#include <libnova/ln_types.h>

#include "bezier.hh"
#include "grid_and_tick.hh"
#include "projection.hh"
#include "track.hh"

class SolarObjectManager;

namespace scene
{

struct Object
{
    CanvasPoint pos;
    double mag;
};

struct LabelledObject
{
    CanvasPoint pos;
    double mag;
    std::string label;
};

struct ProportionalObject
{
    CanvasPoint pos;
    double radius;
    std::string label;
};

struct DirectedObject
{
    CanvasPoint pos, dir;
};

struct Rectangle
{
    CanvasPoint start, size;
};

struct Line
{
    CanvasPoint start, end;
};

struct Path
{
    BezierCurve path;
};

struct Text
{
    std::string body;
    CanvasPoint pos;
};

struct Group;

typedef boost::variant<
    boost::recursive_wrapper<Group>,
    Object,
    ProportionalObject,
    LabelledObject,
    DirectedObject,
    Rectangle,
    Line,
    Path,
    Text
    > Element;

struct Group
{
    std::string class_;
    std::string id;
    std::deque<Element> elements;

    Group(const Group &) = delete;
    Group(Group &&) = default;
};

class Scene
{
    const std::shared_ptr<Projection> projection_;
    Group scene_;

public:
    Scene(const std::shared_ptr<Projection> & projection);
    Scene(const Scene &) = delete;

    void add_group(Group && group);

    template <typename Visitor>
    void apply_visitor(Visitor & v)
    {
        for (auto const & el : scene_.elements)
            boost::apply_visitor(v, el);
    }
};

scene::Group build_grid(
    const Grid grid,
    const std::shared_ptr<Projection> & projection,
    ln_lnlat_posn observer, double t);

scene::Group build_track(
    const Track & track,
    const std::shared_ptr<Projection> & projection,
    const double epoch,
    const SolarObjectManager & solar_manager);

scene::Group build_tick(
    const Tick & tick,
    const std::shared_ptr<Projection> & projection);

scene::Group build_constellations(
    const std::shared_ptr<Projection> & projection,
    const double epoch);

}


#endif
