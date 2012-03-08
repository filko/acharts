#ifndef ACHART_BEZIER_HH
#define ACHART_BEZIER_HH 1

#include <cmath>
#include <vector>

#include "canvas.hh"

struct BezierPoint
{
    CanvasPoint p, cm, cp, perpendicular;
};

typedef std::vector<BezierPoint> BezierCurve;

BezierCurve interpolate_bezier(const std::vector<CanvasPoint> & curve);

#endif
