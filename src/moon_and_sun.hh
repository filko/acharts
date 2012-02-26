#ifndef MOON_AND_SUN_HH
#define MOON_AND_SUN_HH 1

#include "solar_object.hh"

#include <memory>

class MoonOrSun
    : public SolarObject
{
protected:
    struct Implementation;
    std::unique_ptr<Implementation> imp_;

    enum object_type { moon, sun };

    MoonOrSun(object_type type);

public:
    virtual ~MoonOrSun();

    virtual const std::string name() const;

    virtual ln_equ_posn get_equ_coords(double JD) const;
    virtual double get_magnitude(double JD) const;
    virtual double get_sdiam(double JD) const;
};

class Moon
    : public MoonOrSun
{
public:
    Moon()
        : MoonOrSun(moon)
    {
    }
};

class Sun
    : public MoonOrSun
{
public:
    Sun()
        : MoonOrSun(sun)
    {
    }
};

#endif
