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

class Catalogue
{
    typedef std::vector<Star> Stars;
    Stars stars_;

public:
    Catalogue()
    {
    }

    void load(const std::string & filename);

    std::size_t size() const
    {
        return stars_.size();
    }

    const Stars & get_stars() const
    {
        return stars_;
    }
};

#endif
