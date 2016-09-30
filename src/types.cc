/*
 * Copyright (c) 2012-2016 Łukasz P. Michalik <lpmichalik@googlemail.com>

 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:

 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
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
    switch (t.type())
    {
        case timestamp::Type::real:
            os << "Timestamp: " << std::fixed << t.val();
            break;
        case timestamp::Type::t:
            os << "Timestamp: t";
            break;
        case timestamp::Type::now:
            os << "Timestamp: now";
            break;
    }
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
