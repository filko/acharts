#ifndef ACHARTS_TRACK_HH
#define ACHARTS_TRACK_HH 1

#include "now.hh"

struct Track
{
    std::string name;
    double start, end, interval;

    Track()
        : start(Now::get_jd()),
          end(Now::get_jd() + 30.),
          interval(1.0)
    {
    }
};

#endif
