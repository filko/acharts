#include "now.hh"

#include <libnova/julian_day.h>

Now Now::instance;

Now::Now()
{
    ln_get_date_from_sys(&now);
}

const ln_date Now::get()
{
    return instance.now;
}

double Now::get_jd()
{
    return ln_get_julian_day(&instance.now);
}
