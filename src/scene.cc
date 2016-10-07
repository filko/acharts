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
#include "scene.hh"

#include <libnova/transform.h>

#include "constellations.hh"
#include "drawer.hh"

namespace scene
{

Scene::Scene(const std::shared_ptr<Projection> & projection)
    : projection_(projection),
      scene_{"", "root", {}}
{
}

void Scene::add_group(Group && group)
{
    scene_.elements.push_back(std::move(group));
}

namespace
{

ln_equ_posn convert_to_equ(Coordinates coord, std::pair<double, double> in, ln_lnlat_posn observer, double t)
{
    switch(coord)
    {
        case Coordinates::Equatorial:
            return ln_equ_posn{in.first, in.second};
        case Coordinates::Horizontal:
        {
            ln_hrz_posn hin{in.first, in.second};
            ln_equ_posn out;
            ln_get_equ_from_hrz(&hin, &observer, t, &out);
            return out;
        }
        case Coordinates::Ecliptic:
        {
            ln_lnlat_posn hin{in.first, in.second};
            ln_equ_posn out;
            ln_get_equ_from_ecl(&hin, t, &out);
            return out;
        }
    }
}

void create_parallel_grid(
    scene::Group & group, const Grid grid,
    const std::shared_ptr<Projection> & projection,
    ln_lnlat_posn observer, double t)
{
    for (double y{grid.start.val} ; y <= grid.end.val ; y += grid.step.val)
    {
        std::vector<ln_equ_posn> path;
        for (double x{0} ; x < 360.1 ; x += grid.density.val)
        {
            path.push_back(convert_to_equ(grid.coordinates, {x, y}, observer, t));
        }
        auto bezier(create_bezier_from_path(projection, path));
        for (auto const & b : bezier)
            group.elements.push_back(scene::Path{b});
    }
}

void create_meridian_grid(
    scene::Group & group, const Grid grid,
    const std::shared_ptr<Projection> & projection,
    ln_lnlat_posn observer, double t)
{
    for (double x{0} ; x <= 360.1 ; x += grid.step.val)
    {
        std::vector<ln_equ_posn> path;
        for (double y{grid.start.val} ; y <= grid.end.val ; y += grid.density.val)
        {
            path.push_back(convert_to_equ(grid.coordinates, {x, y}, observer, t));
        }
        auto bezier(create_bezier_from_path(projection, path));
        for (auto const & b : bezier)
            group.elements.push_back(scene::Path{b});
    }
}

}

scene::Group build_grid(
    const Grid grid,
    const std::shared_ptr<Projection> & projection,
    ln_lnlat_posn observer, double t)
{
    scene::Group group{"grid", grid.name, {}};
    switch (grid.plane)
    {
        case Plane::Parallel:
        {
            create_parallel_grid(group, grid, projection, observer, t);
            break;
        }
        case Plane::Meridian:
        {
            create_meridian_grid(group, grid, projection, observer, t);
            break;
        }
    }
    return group;
}

scene::Group build_track(
    const Track & track,
    const std::shared_ptr<Projection> & projection,
    const double epoch,
    const SolarObjectManager & solar_manager)
{
    scene::Group group{"track", track.name, {}};
    auto path(create_path_from_track(projection, track, epoch, solar_manager.get(track.name)));
    auto beziers(create_bezier_from_path(path, projection->max_distance()));
    for (auto const & b : beziers)
    {
        group.elements.push_back(scene::Path{b});
    }

    if (path.size() >= 2)
    {
        auto blind_beziered(interpolate_bezier(path));
        for (int i(0), i_end(blind_beziered.size());
             i < i_end; i += track.interval_ticks)
        {
            auto p(blind_beziered[i]);
            group.elements.push_back(scene::DirectedObject{p.p, p.perpendicular});
        }
    }
    return group;
}

scene::Group build_tick(
    const Tick & tick,
    const std::shared_ptr<Projection> & projection,
    ln_lnlat_posn observer, double t)
{
    scene::Group group{"tick", tick.name, {}};

    typedef std::pair<double, double> dpair;
    double dpair::* locked;
    double dpair::* rolling;
    switch (tick.plane)
    {
        case Plane::Parallel:
            locked = &dpair::second;
            rolling = &dpair::first;
            break;
        case Plane::Meridian:
            locked = &dpair::first;
            rolling = &dpair::second;
            break;
    }

    for (double p{tick.start.val} ; p <= tick.end.val ; p += tick.step.val)
    {
        std::string str;
        if (tick.display == Tick::as_hours)
            str = stringify(angle{p}, as_hour);
        else
            str = stringify(angle{p}, as_degree);

        dpair pos;
        pos.*locked = tick.base.val;
        pos.*rolling = p;
        group.elements.push_back(scene::Text{str, projection->project(convert_to_equ(tick.coordinates, pos, observer, t))});
    }

    return group;
}

scene::Group build_constellations(const std::shared_ptr<Projection> & projection, const double epoch)
{
    static constexpr double B1875{2405889.258550475};
    auto pr = [projection,epoch](const ln_equ_posn & in)
    {
        return projection->project(convert_epoch(in, B1875, epoch));
    };

    scene::Group group{"constellations", "all", {}};
    for (auto edge : constellation_edges)
    {
        ln_equ_posn s(edge.first), e(edge.second);
        s.ra *= 15.0; e.ra *= 15.0;

        double ln_equ_posn::* memp;
        if (s.ra == e.ra)
            memp = &ln_equ_posn::dec;
        else
            memp = &ln_equ_posn::ra;

        std::vector<CanvasPoint> path;
        for ( ; s.*memp < e.*memp ; s.*memp += 1.)
            path.push_back(pr(s));
        path.push_back(pr(e));
        auto bezier(create_bezier_from_path(path, projection->max_distance()));
        for (const auto & b : bezier)
            group.elements.push_back(scene::Path{b});
    }
    return group;
}

}
