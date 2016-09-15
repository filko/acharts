#include "scene.hh"

#include <libnova/transform.h>

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

ln_equ_posn hrz_to_equ(ln_hrz_posn in, ln_lnlat_posn observer, double t)
{
    ln_equ_posn out;
    ln_get_equ_from_hrz(&in, &observer, t, &out);
    return std::move(out);
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
            switch (grid.coordinates)
            {
                case Coordinates::Equatorial:
                    path.push_back({x, y});
                    break;
                case Coordinates::Horizontal:
                    path.push_back(hrz_to_equ({x, y}, observer, t));
                    break;
            }
        }
        auto bezier{create_bezier_from_path(projection, path)};
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
            switch (grid.coordinates)
            {
                case Coordinates::Equatorial:
                    path.push_back({x, y});
                    break;
                case Coordinates::Horizontal:
                    ln_hrz_posn in{x, y};
                    ln_equ_posn out;
                    ln_get_equ_from_hrz(&in, &observer, t, &out);
                    path.push_back(out);
                    break;
            }
        }
        auto bezier{create_bezier_from_path(projection, path)};
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
    return std::move(group);
}

scene::Group build_track(
    const Track & track,
    const std::shared_ptr<Projection> & projection,
    const SolarObjectManager & solar_manager)
{
    scene::Group group{"track", track.name, {}};
    auto path(create_path_from_track(projection, track, solar_manager.get(track.name)));
    auto beziers(create_bezier_from_path(path));
    for (auto const & b : beziers)
    {
        group.elements.push_back(scene::Path{b});
    }

    auto blind_beziered(interpolate_bezier(path));
    for (int i(0), i_end(blind_beziered.size());
         i < i_end; i += track.interval_ticks)
    {
        auto p(blind_beziered[i]);
        group.elements.push_back(scene::DirectedObject{p.p, p.perpendicular});
    }
    return group;
}

scene::Group build_tick(
    const Tick & tick,
    const std::shared_ptr<Projection> & projection)
{
    scene::Group group{"tick", tick.name, {}};
    switch (tick.plane)
    {
        case Plane::Parallel:
            for (double x{tick.start.val} ; x <= tick.end.val ; x += tick.step.val)
            {
                std::string str;
                if (tick.display == Tick::as_hours)
                    str = stringify(angle{x}, as_hour);
                else
                    str = stringify(angle{x}, as_degree);

                group.elements.push_back(scene::Text{str, projection->project({x, tick.base.val})});
            }
            break;
        case Plane::Meridian:
            for (double y{tick.start.val} ; y <= tick.end.val ; y += tick.step.val)
            {
                std::string str;
                if (tick.display == Tick::as_hours)
                    str = stringify(angle{y}, as_hour);
                else
                    str = stringify(angle{y}, as_degree);

                group.elements.push_back(scene::Text{str, projection->project({tick.base.val, y})});
            }
            break;
    }
    return group;
}

}
