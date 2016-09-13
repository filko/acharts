#include "drawer.hh"

#include <cmath>
#include <cstring>
#include <deque>
#include <fstream>
#include <stdexcept>

#include "bezier.hh"

const std::vector<CanvasPoint> create_path_from_track(const std::shared_ptr<Projection> & projection,
                                                      const Track & track, const std::shared_ptr<const SolarObject> & object)
{
    std::vector<CanvasPoint> path;
    double step(track.mark_interval / track.interval_ticks);

    for (double t(track.start.val()); t <= track.end.val(); t += step)
    {
        auto coord(object->get_equ_coords(t));
        path.push_back(projection->project(coord));
    }

    return path;
}

const std::deque<BezierCurve> create_bezier_from_path(const std::shared_ptr<Projection> & projection, const std::vector<ln_equ_posn> & path)
{
    std::vector<CanvasPoint> ret;
    for (auto const & p : path)
    {
        ret.push_back(projection->project(p));
    }
    return create_bezier_from_path(ret);
}

const std::deque<BezierCurve> create_bezier_from_path(const std::vector<CanvasPoint> & path)
{
    {
        std::vector<std::vector<CanvasPoint>> input(1);
        for (auto const & p : path)
        {
            if (! p.nan())
                input.back().push_back(p);
            else if (! input.back().empty())
                input.push_back(std::vector<CanvasPoint>());
        }

        std::deque<BezierCurve> ret;
        for (auto const & b : input)
        {
            if (b.size() < 2)
                continue;

            ret.push_back(interpolate_bezier(b));
        }

        return ret;
    }
}
