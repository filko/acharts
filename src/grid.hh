#ifndef ACHARTS_GRID_HH
#define ACHARTS_GRID_HH 1

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

#endif
