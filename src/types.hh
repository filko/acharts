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
