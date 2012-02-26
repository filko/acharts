#include "planet.hh"

#include <libnova/mercury.h>
#include <libnova/venus.h>
#include <libnova/mars.h>
#include <libnova/jupiter.h>
#include <libnova/saturn.h>
#include <libnova/uranus.h>
#include <libnova/neptune.h>
#include <libnova/pluto.h>

#include "exceptions.hh"

typedef void (* get_equ_coords_t)(double, ln_equ_posn *);
typedef double (* get_magnitude_t)(double);

struct Planet::Implementation
{
    get_equ_coords_t get_equ_coords;
    get_magnitude_t get_magnitude;

    std::string name;

    Implementation(const std::string & name)
        : name(name)
    {
        this->name[0] = std::toupper(this->name[0]);
    }
};

Planet::Planet(const std::string & name)
    : imp_(new Implementation(name))
{
#define MAKE_PLANET(planet) \
    if (#planet == name) \
    { \
        imp_->get_equ_coords = &ln_get_##planet##_equ_coords; \
        imp_->get_magnitude = &ln_get_##planet##_magnitude;   \
    }

    MAKE_PLANET(mercury)
    else
    MAKE_PLANET(venus)
    else
    MAKE_PLANET(mars)
    else
    MAKE_PLANET(jupiter)
    else
    MAKE_PLANET(saturn)
    else
    MAKE_PLANET(uranus)
    else
    MAKE_PLANET(neptune)
    else
    MAKE_PLANET(pluto)
    else
        throw ConfigError("Unknown planet: " + name);

#undef MAKE_PLANET
}

Planet::~Planet()
{
}

const std::string Planet::name() const
{
    return imp_->name;
}

ln_equ_posn Planet::get_equ_coords(double JD) const
{
    ln_equ_posn ret;
    imp_->get_equ_coords(JD, &ret);
    return ret;
}

double Planet::get_sdiam(double) const
{
    return 0.;
}

double Planet::get_magnitude(double JD) const
{
    return imp_->get_magnitude(JD);
}
