#ifndef CHARTS_PLANET_HH
#define CHARTS_PLANET_HH 1

#include <libnova/ln_types.h>
#include <memory>

class Planet
{
    struct Implementation;
    std::unique_ptr<Implementation> imp_;

public:
    explicit Planet(const std::string & name);
    ~Planet();
    Planet(const Planet &) = delete;
    Planet & operator=(const Planet &) = delete;

    const std::string name() const;

    ln_equ_posn get_equ_coords(double JD) const;
    double get_magnitude(double JD) const;
    double get_sdiam(double JD) const;
};

#endif
