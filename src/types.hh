#ifndef ACHARTS_TYPES_HH
#define ACHARTS_TYPES_HH 1

#include <string>

struct angle
{
    double val;
};
std::ostream & operator<<(std::ostream & os, const angle & a);

extern struct as_hour_t {} as_hour;
extern struct as_degree_t {} as_degree;

std::string stringify(angle a, as_hour_t);
std::string stringify(angle a, as_degree_t);
std::string stringify(angle a);

struct timestamp
{
    double val;
};
std::ostream & operator<<(std::ostream & os, const timestamp & t);

struct length
{
    double val;
};
std::ostream & operator<<(std::ostream & os, const length & s);

struct boolean
{
    bool val;
};
std::ostream & operator<<(std::ostream & os, const boolean & b);

#endif
