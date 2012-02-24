#ifndef CHARTS_NOW_HH
#define CHARTS_NOW_HH 1

#include <libnova/ln_types.h>

class Now
{
private:
    Now();
    Now(const Now &) = delete;
    Now & operator=(const Now &) = delete;

    ln_date now;

    static Now instance;

public:
    static const ln_date get();
    static double get_jd();
};

#endif
