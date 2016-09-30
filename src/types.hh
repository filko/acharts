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
#ifndef ACHARTS_TYPES_HH
#define ACHARTS_TYPES_HH 1

#include <string>
#include <stdexcept>

struct angle
{
    double val;
};
std::ostream & operator<<(std::ostream & os, const angle & a);

extern struct as_hour_t {} as_hour;
extern struct as_degree_t {} as_degree;

std::string stringify(angle a, as_hour_t);
std::string stringify(angle a, as_degree_t);
std::string stringify(angle a);

class timestamp
{
public:
    struct Now {};
    struct T {};
    enum class Type
    {
        real,
        now,
        t
    };
    timestamp() : val_(0), type_(Type::now) { }
    explicit timestamp(double val) : val_(val), type_(Type::real) { }
    explicit timestamp(Type type) : val_(0), type_(type) { }
    timestamp(Now) : val_(0), type_(Type::now) { }
    timestamp(T) : val_(0), type_(Type::t) { }
    timestamp & operator=(double val) { val_ = val, type_ = Type::real; return *this; }

    double val() const
    {
        if (Type::real != type_)
            throw std::logic_error("calling val() on non-real timestamp");

        return val_;
    }

    void val(double v)
    {
        val_ = v;
        type_ = Type::real;
    }

    Type type() const
    {
        return type_;
    }

private:
    double val_;
    Type type_;
};
std::ostream & operator<<(std::ostream & os, const timestamp & t);

struct length
{
    double val;
};
std::ostream & operator<<(std::ostream & os, const length & s);

struct boolean
{
    bool val;
};
std::ostream & operator<<(std::ostream & os, const boolean & b);

struct integer
{
    int val;
};
std::ostream & operator<<(std::ostream & os, const integer & i);

#endif
