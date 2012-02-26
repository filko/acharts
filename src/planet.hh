#ifndef CHARTS_PLANET_HH
#define CHARTS_PLANET_HH 1

#include "solar_object.hh"

#include <libnova/ln_types.h>
#include <memory>

class Planet
    : public SolarObject
{
    struct Implementation;
    std::unique_ptr<Implementation> imp_;

public:
    explicit Planet(const std::string & name);
    virtual ~Planet();

    virtual const std::string name() const;

    virtual ln_equ_posn get_equ_coords(double JD) const;
    virtual double get_magnitude(double JD) const;
    virtual double get_sdiam(double JD) const;
};

#endif