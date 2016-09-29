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

    for (double t(0); t <= track.length.val(); t += step)
    {
        auto coord(object->get_equ_coords(track.start.val() + t));
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
    return create_bezier_from_path(ret, projection->max_distance());
}

namespace
{

void cut_paths_when_distance_is_big(std::vector<std::vector<CanvasPoint>> & input, double max_distance)
{
    auto iter(input.begin());
    while (input.end() != iter)
    {
        if (iter->size() < 2)
        {
            iter = input.erase(iter);
            continue;
        }

        auto el2(iter->begin());
        auto el1(el2++);
        auto iter2(iter);
        ++iter;
        while (iter2->end() != el2)
        {
            double distance((*el1 - *el2).norm());
            if (distance > max_distance)
            {
                std::vector<CanvasPoint> v2(std::make_move_iterator(el2), std::make_move_iterator(iter2->end()));
                iter2->erase(el2, iter2->end());
                ++iter2;
                iter = input.insert(iter2, std::move(v2));
                break;
            }
            ++el1, ++el2;
        }
    }
}

}

const std::deque<BezierCurve> create_bezier_from_path(const std::vector<CanvasPoint> & path, double max_distance)
{
    std::vector<std::vector<CanvasPoint>> input(1);
    for (auto const & p : path)
    {
        if (! p.nan())
            input.back().push_back(p);
        else if (! input.back().empty())
            input.push_back(std::vector<CanvasPoint>());
    }

    cut_paths_when_distance_is_big(input, max_distance);

    std::deque<BezierCurve> ret;
    for (auto const & b : input)
    {
        if (b.size() < 2)
            continue;

        ret.push_back(interpolate_bezier(b));
    }

    return ret;
}
