#ifndef ACHARTS_GRID_AND_TICK_HH
#define ACHARTS_GRID_AND_TICK_HH 1

#include "types.hh"

enum class Coordinates
{
    Equatorial,
    Horizontal,
};

enum class Plane
{
    Parallel,
    Meridian
};

struct Grid
{
    Coordinates coordinates;
    Plane plane;
    std::string name;
    angle start, end, step, density = {2};
};

struct Tick
{
    Coordinates coordinates;
    Plane plane;
    std::string name;
    enum Type { as_degrees, as_hours } display = as_degrees;
    angle start, end, step, base = {0};
};

#endif
