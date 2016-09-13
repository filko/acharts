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
                                                      const Track & track, const std::shared_ptr<const SolarObject> & object);

const std::deque<BezierCurve> create_bezier_from_path(const std::vector<CanvasPoint> & path);
const std::deque<BezierCurve> create_bezier_from_path(const std::shared_ptr<Projection> & projection, const std::vector<ln_equ_posn> & path);

#endif
