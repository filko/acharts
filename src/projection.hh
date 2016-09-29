#ifndef GRAPH_PROJECTION_HH
#define GRAPH_PROJECTION_HH 1

#include <libnova/ln_types.h>
#include <memory>

#include "canvas.hh"

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
    Projection(const CanvasPoint & canvas, const ln_equ_posn & apparent_canvas, const ln_equ_posn & center);

public:
    virtual ~Projection();
    virtual CanvasPoint project(const ln_equ_posn & pos) const = 0;
    virtual double scale_at_point(const ln_equ_posn & pos) const;
    virtual void rotate_to_level(const CanvasPoint & pos);
    double max_distance() const;

protected:
    CanvasPoint canvas_;
    SphericalCoord apparent_canvas_;
    SphericalCoord center_;
    double rotationSin_, rotationCos_;
};

class ProjectionFactory
{
public:
    static std::shared_ptr<Projection> create(const std::string & type,
                                              const CanvasPoint & canvas, const ln_equ_posn & apparent_canvas,
                                              const ln_equ_posn & center);
};

ln_equ_posn convert_epoch(ln_equ_posn in, double fromJD, double toJD);

#endif
