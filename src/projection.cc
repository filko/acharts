#include "projection.hh"

#include <boost/algorithm/string.hpp>
#include <cmath>
#include <iostream>
#include <libnova/utility.h>
#include <stdexcept>

#include "exceptions.hh"

SphericalCoord::SphericalCoord(const ln_equ_posn & rh)
    : ra(ln_deg_to_rad(rh.ra)),
      dec(ln_deg_to_rad(rh.dec))
{
}

Projection::Projection(const CanvasPoint & canvas, const ln_equ_posn & apparent_canvas, const ln_equ_posn & center)
    : canvas_(canvas), apparent_canvas_(apparent_canvas), center_(center)
{
    if (0. == apparent_canvas_.ra)
    {
        if (0. == apparent_canvas_.dec)
            throw std::runtime_error("Need to specify at least one apparent size dimention non-zero!");

        apparent_canvas_.ra = apparent_canvas_.dec * canvas_.x / canvas_.y;
    }
    else if (0. == apparent_canvas_.dec)
    {
        apparent_canvas_.dec = apparent_canvas_.ra * canvas_.y / canvas_.x;
    }
}

Projection::~Projection()
{
}

double Projection::scale_at_point(const ln_equ_posn & pos) const
{
    ln_equ_posn p2(pos);
    p2.ra += 0.1;
    CanvasPoint o1(this->project(pos)), o2(this->project(p2));
    return (o1.x - o2.x) * 10.;
}

class AzimuthalEquidistantProjection
    : public Projection
{
public:
    AzimuthalEquidistantProjection(const CanvasPoint & canvas, const ln_equ_posn & apparent_canvas,
                                   const ln_equ_posn & center)
        : Projection(canvas, apparent_canvas, center)
    {
    }

    virtual CanvasPoint project(const ln_equ_posn & pos) const
    {
        double ra(ln_deg_to_rad(pos.ra)), dec(ln_deg_to_rad(pos.dec));
        double cosc(sin(center_.dec) * sin(dec) + cos(center_.dec) * cos(dec) * cos(ra - center_.ra));
        double c(acos(cosc));
        double k;
        if (c == 0)
            k = 1;
        else
            k = c / sin(c);
        double x(k * cos(dec) * sin(ra - center_.ra));
        double y(k * (cos(center_.dec) * sin(dec) - sin(center_.dec) * cos(dec) * cos(ra - center_.ra)));

        // negate x, because for svg up is down
        // negate y, because right ascention is going left to right
        return CanvasPoint(-x * canvas_.x / apparent_canvas_.ra,
                           -y * canvas_.y / apparent_canvas_.dec);
    }
};

std::shared_ptr<Projection> ProjectionFactory::create(const std::string & type,
                                                      const CanvasPoint & canvas, const ln_equ_posn & apparent_canvas,
                                                      const ln_equ_posn & center)
{
    std::string t(boost::algorithm::to_lower_copy(type));

    if ("azimuthalequidistant" == t)
        return std::make_shared<AzimuthalEquidistantProjection>(canvas, apparent_canvas, center);

    throw ConfigError("Unknown projection specified: " + type);
}
