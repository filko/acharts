#ifndef ACHARTS_TRACK_HH
#define ACHARTS_TRACK_HH 1

#include "now.hh"

struct Track
{
    std::string name;
    double start, end, mark_interval;
    int interval_ticks;

    Track()
        : start(Now::get_jd()),
          end(Now::get_jd() + 30.),
          mark_interval(7.0),
          interval_ticks(7)
    {
    }
};

#endif
