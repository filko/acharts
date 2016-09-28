#ifndef ACHARTS_TRACK_HH
#define ACHARTS_TRACK_HH 1

#include "now.hh"
#include "types.hh"

struct Track
{
    std::string name;
    timestamp start, length;
    double mark_interval;
    int interval_ticks;

    Track()
        : start(Now::get_jd()),
          length(30.),
          mark_interval(7.0),
          interval_ticks(7)
    {
    }
};

#endif
