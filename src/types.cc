#include "types.hh"

#include <libnova/utility.h>
#include <sstream>

std::ostream & operator<<(std::ostream & os, const angle & a)
{
    os << "Angle: " << a.val;
    return os;
}

std::string stringify(angle a, as_hour_t)
{
    ln_hms hms;
    ln_deg_to_hms(a.val, &hms);
    std::ostringstream os;
    os << hms.hours << 'h';
    if (0 != hms.minutes || 0. != hms.seconds)
    {
        os << hms.minutes << 'm';
        if (0. != hms.seconds)
            os << hms.seconds << 's';
    }
    return os.str();
}

std::string stringify(angle a, as_degree_t)
{
    ln_dms dms;
    ln_deg_to_dms(a.val, &dms);
    std::ostringstream os;
    if (dms.neg)
        os << '-';
    os << dms.degrees << "°";
    if (0 != dms.minutes || 0. != dms.seconds)
    {
        os << dms.minutes << "′";
        if (0. != dms.seconds)
            os << dms.seconds << "″";
    }
    return os.str();
}

std::string stringify(angle a)
{
    std::ostringstream os;
    os << a;
    return os.str();
}

std::ostream & operator<<(std::ostream & os, const timestamp & t)
{
    auto flags(os.flags());
    os << "Timestamp: " << std::fixed << t.val;
    os.flags(flags);
    return os;
}

std::ostream & operator<<(std::ostream & os, const length & s)
{
    os << "Length: " << s.val << " mm";
    return os;
}

std::ostream & operator<<(std::ostream & os, const boolean & b)
{
    os << (b.val ? "on" : "off");
    return os;
}

std::ostream & operator<<(std::ostream & os, const integer & i)
{
    os << i.val;
    return os;
}
