#include "bezier.hh"

#include "exceptions.hh"

namespace
{

BezierPoint bezier_ending(const CanvasPoint & first, const CanvasPoint & second)
{
    CanvasPoint mirror(second - first);
    CanvasPoint perpendicular(-mirror.y, mirror.x);
    perpendicular /= perpendicular.norm();
    // we push endings just a little in the direction of next/prev points
    return BezierPoint{ first, first - 0.1 * mirror, first + 0.1 * mirror, perpendicular };
}

}

BezierCurve interpolate_bezier(const std::vector<CanvasPoint> & curve)
{
    BezierCurve ret;

    if (curve.size() < 2)
        throw InternalError("Trying to bezierize a curve of length " + std::to_string(curve.size()));

    auto iAm(curve.cbegin()), iA(iAm + 1);

    ret.push_back(bezier_ending(*iAm, *iA));

    if (curve.size() >= 3)
    {
        for (auto iAp(iA + 1), iA_end(curve.end());
             iAp != iA_end; iAm = iA, iA = iAp, ++iAp)
        {
            CanvasPoint Am(*iAm);
            CanvasPoint A(*iA);
            CanvasPoint Ap(*iAp);

            CanvasPoint am((Am + A) / 2.);
            CanvasPoint ap((A + Ap) / 2.);

            double Lm = (A - Am).norm() / 2.;
            double Lp = (Ap - A).norm() / 2.;

            double d = (ap - am).norm();
            CanvasPoint de = (Ap - Am) / (Ap - Am).norm();

            double b = d * Lm / (Lm + Lp);
            double u = d * Lp / (Lm + Lp);

            CanvasPoint Cm = A - de * b;
            CanvasPoint Cp = A + de * u;
            CanvasPoint perpendicular{-de.y, de.x};

            ret.push_back(BezierPoint{A, Cm, Cp, perpendicular});
        }
    }

    auto e(bezier_ending(*iA, *iAm));
    std::swap(e.cm, e.cp);
    ret.push_back(e);

    return ret;
}
