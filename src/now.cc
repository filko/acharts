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
