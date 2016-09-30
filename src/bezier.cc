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
#include "bezier.hh"

#include "exceptions.hh"

namespace
{

BezierPoint mirror_bezier(const CanvasPoint & A, const CanvasPoint & B, const CanvasPoint & Bprim)
{
    CanvasPoint O((A + B) / 2.);
    CanvasPoint n(A - B);
    n = CanvasPoint{-n.y, n.x};
    n /= n.norm();

    CanvasPoint Aprim(-Bprim + 2 * O + 2 * n * ((Bprim - O) * n));
    CanvasPoint diff(Aprim - A);
    CanvasPoint perpendicular(-diff.y, diff.x);
    perpendicular /= perpendicular.norm();
    return BezierPoint{ A, A - diff, Aprim, perpendicular };
}

}

BezierCurve interpolate_bezier(const std::vector<CanvasPoint> & curve)
{
    BezierCurve ret;

    if (curve.size() < 2)
        throw InternalError("Trying to bezierize a curve of length " + std::to_string(curve.size()));

    auto iAm(curve.cbegin()), iA(iAm + 1);

    if (curve.size() >= 3)
    {
        {
            BezierPoint b;
            b.p = *iAm;
            ret.push_back(b);
        }

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

        ret[0] = mirror_bezier(ret[0].p, ret[1].p, ret[1].cm);
        auto e(mirror_bezier(*iA, ret.back().p, ret.back().cp));
        std::swap(e.cm, e.cp);
        ret.push_back(e);
    }
    else
    {
        CanvasPoint A(*iAm), B(*iA);
        CanvasPoint mirror(B - A);
        CanvasPoint perpendicular(-mirror.y, mirror.x);
        perpendicular /= perpendicular.norm();

        ret.push_back(BezierPoint{A, A - mirror, B, perpendicular});
        ret.push_back(BezierPoint{B, A, B + mirror, perpendicular});
    }

    return ret;
}
