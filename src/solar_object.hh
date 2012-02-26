#ifndef SOLAR_OBJECT_HH
#define SOLAR_OBJECT_HH 1

#include <libnova/ln_types.h>
#include <string>

class SolarObject
{
protected:
    SolarObject() = default;

public:
    virtual ~SolarObject() { };
    SolarObject(const SolarObject &) = delete;
    SolarObject & operator=(const SolarObject &) = delete;

    virtual const std::string name() const = 0;

    virtual ln_equ_posn get_equ_coords(double JD) const = 0;
    virtual double get_magnitude(double JD) const = 0;
    virtual double get_sdiam(double JD) const = 0;
};

#endif
