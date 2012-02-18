#ifndef CHARTS_STARS_HH
#define CHARTS_STARS_HH 1

#include <libnova/libnova.h>
#include <string>
#include <vector>

struct Star
{
    std::string common_name_;
    ln_equ_posn pos_;
    double vmag_;

    Star(const std::string & cname,
         lnh_equ_posn & hpos,
         double vm);

    struct by_mag
    {
        bool operator()(const Star & lh, const Star & rh)
        {
            return lh.vmag_ < rh.vmag_;
        }
    };
};

#endif
