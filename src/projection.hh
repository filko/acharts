#ifndef GRAPH_PROJECTION_HH
#define GRAPH_PROJECTION_HH 1

#include <libnova/ln_types.h>
#include <memory>

struct OutputCoord
{
    double x, y;

    OutputCoord()
        : x(0), y(0)
    {
    }

    OutputCoord(double x, double y)
        : x(x), y(y)
    {
    }
};

struct SphericalCoord
{
    double ra, dec;

    SphericalCoord(const ln_equ_posn & rh);
};

class Projection
{
    Projection(const Projection &) = delete;
    Projection & operator=(const Projection &) = delete;

protected:
    Projection(const OutputCoord & canvas, const ln_equ_posn & apparent_canvas, const ln_equ_posn & center);

public:
    virtual ~Projection();
    virtual OutputCoord project(const ln_equ_posn & pos) const = 0;
    virtual double scale_at_point(const ln_equ_posn & pos) const;

protected:
    OutputCoord canvas_;
    SphericalCoord apparent_canvas_;
    SphericalCoord center_;
};

class ProjectionFactory
{
public:
    static std::shared_ptr<Projection> create(const std::string & type,
                                              const OutputCoord & canvas, const ln_equ_posn & apparent_canvas,
                                              const ln_equ_posn & center);
};

#endif
