#include "moon_and_sun.hh"

#include <libnova/lunar.h>
#include <libnova/solar.h>

#include "exceptions.hh"

typedef void (* get_equ_coords_t)(double, ln_equ_posn *);
typedef double (* get_sdiam_t)(double);

struct MoonOrSun::Implementation
{
    get_equ_coords_t get_equ_coords;
    get_sdiam_t get_sdiam;

    std::string name;

    Implementation(object_type type)
    {
        switch (type)
        {
            case moon:
                get_equ_coords = &ln_get_lunar_equ_coords;
                get_sdiam = &ln_get_lunar_sdiam;
                name = "Moon";
                break;
            case sun:
                get_equ_coords = &ln_get_solar_equ_coords;
                get_sdiam = &ln_get_solar_sdiam;
                name = "Sun";
                break;
        }
    }
};

MoonOrSun::MoonOrSun(object_type type)
    : imp_(new Implementation(type))
{
}

MoonOrSun::~MoonOrSun()
{
}

const std::string MoonOrSun::name() const
{
    return imp_->name;
}

ln_equ_posn MoonOrSun::get_equ_coords(double JD) const
{
    ln_equ_posn ret;
    imp_->get_equ_coords(JD, &ret);
    return ret;
}

double MoonOrSun::get_sdiam(double JD) const
{
    return imp_->get_sdiam(JD);
}

double MoonOrSun::get_magnitude(double) const
{
    return 0.;
}
