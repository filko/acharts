#include "config_parser.hh"

// #define BOOST_SPIRIT_DEBUG 1

#define BOOST_SPIRIT_USE_PHOENIX_V3 1

#include <cmath>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/include/adapt_adt.hpp>
#include <boost/phoenix/fusion/at.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>
#include <boost/spirit/include/qi_auxiliary.hpp>
#include <boost/spirit/include/qi_core.hpp>
#include <boost/variant.hpp>
#include <libnova/julian_day.h>
#include <libnova/ln_types.h>

#include "catalogue_description.hh"
#include "exceptions.hh"
#include "now.hh"

typedef std::pair<std::string, std::string> pair_type;
typedef boost::variant<std::string, pair_type> config_variant;

#ifdef BOOST_SPIRIT_DEBUG
    namespace std
    {
    std::ostream & operator<<(std::ostream & os, const pair_type & p)
    {
        os << p.first << ": " << p.second;
        return os;
    }
    }
#endif

namespace config_parser
{

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace spirit = boost::spirit;
namespace ascii = spirit::ascii;
namespace qi = spirit::qi;

using ascii::alpha;
using ascii::alnum;
using ascii::blank;
using ascii::char_;
using ascii::graph;
using ascii::space;
using spirit::_1;
using spirit::_2;
using spirit::_3;
using spirit::_4;
using spirit::_5;
using spirit::_val;
using spirit::as_string;
using spirit::double_;
using spirit::eoi;
using spirit::eol;
using spirit::int_;
using spirit::lexeme;
using spirit::lit;
using spirit::omit;
using spirit::skip;

struct parser_proxy
    : boost::static_visitor<>
{
    typedef config_variant value_type;

    SectionCallback scal_;
    ValueCallback vcal_;

    std::string current_name;

    parser_proxy(const SectionCallback & scal, const ValueCallback & vcal)
        : scal_(scal),
          vcal_(vcal),
          current_name("core")
    {
    }

    void push_back(const config_variant & v)
    {
        boost::apply_visitor(*this, v);
    }

    void operator()(const std::string & section)
    {
        current_name = section;
        scal_(section);
    }

    void operator()(const pair_type & p)
    {
        vcal_(current_name + '.' + p.first, p.second);
    }

    config_variant & get() const
    {
        static config_variant p;
        return p;
    }
};

template <typename Iterator, typename Skipper>
class config_grammar
    : public qi::grammar<Iterator, parser_proxy(), Skipper>
{
    struct error_handler_imp
    {
        typedef void result_type;

        Iterator begin;

        explicit error_handler_imp(Iterator b)
            : begin(b)
        {
        }

        void operator()(Iterator first, Iterator last,
                        Iterator err_pos, const boost::spirit::info &) const
        {
            std::ostringstream os;
            os << "Parsing failed at line " << spirit::get_line(err_pos)
               << ", near: " << spirit::get_current_line(begin, first, last);
            throw ConfigError(os.str());
        }
    };

public:
    config_grammar(Iterator begin)
        : config_grammar::base_type(config),
          error_handler(error_handler_imp(begin))

    {
        identifier =
            alpha[_val += _1] >> *(char_(".-") | alnum)[_val += _1];

        value %=
            lexeme['"' > *~ascii::char_('"') > '"'] |
            +(graph - '"');

        entry_rule =
            identifier > value > (eol | eoi);

        section =
            "[" > identifier > "]";

        config %=
            *(section | skip(blank)[entry_rule]);

        qi::on_error<qi::fail>(value, error_handler(_1, _2, _3, _4));

#ifdef BOOST_SPIRIT_DEBUG
        BOOST_SPIRIT_DEBUG_NODE(identifier);
        BOOST_SPIRIT_DEBUG_NODE(value);
        BOOST_SPIRIT_DEBUG_NODE(entry_rule);
        BOOST_SPIRIT_DEBUG_NODE(section);
        BOOST_SPIRIT_DEBUG_NODE(config);
#endif
    }

    qi::rule<Iterator, parser_proxy(), Skipper> config;
    qi::rule<Iterator, std::string()> identifier, value, section;
    qi::rule<Iterator, pair_type(), ascii::blank_type> entry_rule;

    phoenix::function<error_handler_imp> error_handler;
};

void parse_config(std::istream & is, const SectionCallback & scal, const ValueCallback & vcal)
{
    std::stringstream ss;
    ss << is.rdbuf();
    std::string s(ss.str());
    spirit::line_pos_iterator<decltype(s.cbegin())> begin(s.cbegin()), iter(begin), iter_end(s.cend());
    typedef decltype(iter) Iter;

    auto skipper(boost::proto::deep_copy(ascii::space | '#' >> *(qi::char_ - qi::eol) >> qi::eol));
    config_grammar<Iter, decltype(skipper)> pars(begin);
    parser_proxy t(scal, vcal);
    try
    {
        bool r(qi::phrase_parse(iter, iter_end, pars, skipper, t));
        if (! r or iter != iter_end)
        {
            throw ConfigError("parser failed");
        }
    }
    catch (const spirit::qi::expectation_failure<Iter> & x)
    {
        auto range(spirit::get_current_line(begin, x.first, iter_end));
        std::ostringstream os;
        os << "Parsing failed at line: " << x.first.position() << ": " << range;
        throw ConfigError(os.str());
    }
}

const double rad2deg(45. / std::atan(1.));

template <typename Iterator>
class angle_grammar
    : public qi::grammar<Iterator, double()>
{
public:
    angle_grammar()
        : angle_grammar::base_type(start)
    {
        radians = double_[_val = rad2deg * _1] >> 'r';
        degrees = double_[_val = _1] >> -('d' >>
            -(double_[_val += _1 / 60.] >> 'm' >>
              -(double_[_val += _1/ 3600.] >> 's')));
        hours = double_[_val = 15 * _1] >> 'h' >>
            -(double_[_val += _1 / 4.] >> 'm' >>
              -(double_[_val += _1 / 240. ] >> 's'));
        start = radians | hours | degrees;
    }

    qi::rule<Iterator, double()> start;
    qi::rule<Iterator, double()> radians;
    qi::rule<Iterator, double()> degrees;
    qi::rule<Iterator, double()> hours;
};

double parse_angle(const std::string & in)
{
    angle_grammar<std::string::const_iterator> pars;
    double ret;
    auto begin(in.cbegin()), end(in.cend());
    bool r(parse(begin, end, pars, ret));
    if (!r or begin != end)
    {
        throw ConfigValueError("angle", in);
    }
    return ret;
}

struct julianepoch_to_juliandate_imp
{
    typedef double result_type;

    double operator()(double epoch) const
    {
        return (epoch - 2000.0) * 365.25 + 2451545.0;
    }
};
phoenix::function<julianepoch_to_juliandate_imp> julianepoch_to_juliandate;

struct besselianepoch_to_juliandate_imp
{
    typedef double result_type;

    double operator()(double epoch) const
    {
        return (epoch - 1900.0) * 365.242198781 + 2415020.31352;
    }
};
phoenix::function<besselianepoch_to_juliandate_imp> besselianepoch_to_juliandate;

struct parseddate_to_juliandate_imp
{
    typedef double result_type;

    double operator()(int h, int m, boost::optional<double> s,
                      boost::optional<boost::fusion::vector3<int, int, int>> date,
                      boost::optional<double> tz) const
    {
        ln_zonedate zdate;

        if (date)
        {
            using boost::fusion::at_c;
            zdate.days = at_c<2>(*date);
            zdate.months = at_c<1>(*date);
            zdate.years = at_c<0>(*date);
        }
        else
        {
            ln_date now(Now::get());
            zdate.days = now.days;
            zdate.months = now.months;
            zdate.years = now.years;
        }

        if (! s)
            s = 0;

        zdate.seconds = *s;
        zdate.minutes = m;
        zdate.hours = h;

        if (tz)
            zdate.gmtoff = *tz * 3600;
        else
            zdate.gmtoff = 0;

        return ln_get_julian_local_date(&zdate);
    }
};
phoenix::function<parseddate_to_juliandate_imp> parseddate_to_juliandate;

struct now_get_imp
{
    typedef double result_type;

    double operator()() const
    {
        return Now::get_jd();
    }
};
phoenix::function<now_get_imp> now_get;

template <typename Iterator>
class timestamp_grammar
    : public qi::grammar<Iterator, timestamp()>
{
public:
    timestamp_grammar()
        : timestamp_grammar::base_type(start)
    {
        using namespace boost::spirit::qi;
        start = (double_ >> "JD") [_val = _1]
            | ("J" > double_) [_val = julianepoch_to_juliandate(_1)]
            | ("B" > double_) [_val = besselianepoch_to_juliandate(_1)]
            | (
                int_ >> ":" >> int_ >> -(":" >> double_) >>
                -(omit[space] >> int_ >> "-" >> int_ >> "-" >> int_) >>
                -(omit[space] >> double_)
                ) [_val = parseddate_to_juliandate(_1, _2, _3, _4, _5)]
            | lit("now") [_val = timestamp::Now()]
            | lit("t") [_val = timestamp::T()]
            ;
    }

    qi::rule<Iterator, timestamp()> start;
};

timestamp parse_timestamp(const std::string & in)
{
    timestamp_grammar<std::string::const_iterator> pars;
    timestamp ret;
    auto begin(in.cbegin()), end(in.cend());
    bool r(parse(begin, end, pars, ret));
    if (!r or begin != end)
    {
        throw ConfigValueError("timestamp", in);
    }
    return ret;
}

template <typename Iterator>
class length_grammar
    : public qi::grammar<Iterator, double()>
{
public:
    length_grammar()
        : length_grammar::base_type(start)
    {
        using namespace boost::spirit::qi;
        start %= (double_ >> -lit("mm"));
    }

    qi::rule<Iterator, double()> start;
};

double parse_length(const std::string & in)
{
    length_grammar<std::string::const_iterator> pars;
    double ret;
    auto begin(in.cbegin()), end(in.cend());
    bool r(parse(begin, end, pars, ret));
    if (!r or begin != end)
    {
        throw ConfigValueError("length", in);
    }
    return ret;
}

template <typename Iterator>
class boolean_grammar
    : public qi::grammar<Iterator, bool()>
{
public:
    boolean_grammar()
        : boolean_grammar::base_type(start)
    {
        using namespace boost::spirit::qi;
        start = lit("on")[_val = true] | lit("off")[_val = false];
    }

    qi::rule<Iterator, bool()> start;
};

bool parse_boolean(const std::string & in)
{
    boolean_grammar<std::string::const_iterator> pars;
    bool ret;
    auto begin(in.cbegin()), end(in.cend());
    bool r(parse(begin, end, pars, ret));
    if (!r or begin != end)
    {
        throw ConfigValueError("boolean", in);
    }
    return ret;
}

template <typename Iterator>
class integer_grammar
    : public qi::grammar<Iterator, int()>
{
public:
    integer_grammar()
        : integer_grammar::base_type(start)
    {
        using namespace boost::spirit::qi;
        start %= int_;
    }

    qi::rule<Iterator, int()> start;
};

int parse_integer(const std::string & in)
{
    integer_grammar<std::string::const_iterator> pars;
    int ret;
    auto begin(in.cbegin()), end(in.cend());
    bool r(parse(begin, end, pars, ret));
    if (!r or begin != end)
    {
        throw ConfigValueError("integer", in);
    }
    return ret;
}

template <typename Iterator>
class double_grammar
    : public qi::grammar<Iterator, double()>
{
public:
    double_grammar()
        : double_grammar::base_type(start)
    {
        using namespace boost::spirit::qi;
        start %= double_;
    }

    qi::rule<Iterator, double()> start;
};

double parse_double(const std::string & in)
{
    double_grammar<std::string::const_iterator> pars;
    double ret;
    auto begin(in.cbegin()), end(in.cend());
    bool r(parse(begin, end, pars, ret));
    if (!r or begin != end)
    {
        throw ConfigValueError("double", in);
    }
    return ret;
}

struct string_to_field_imp
{
    typedef CatalogParsingDescription::Field result_type;

    result_type operator()(const std::string & in) const
    {
#define CF CatalogParsingDescription::Field
        if ("Name" == in)
            return CF::Name;
        else if ("RAh" == in)
            return CF::RAh;
        else if ("RAm" == in)
            return CF::RAm;
        else if ("RAs" == in)
            return CF::RAs;
        else if ("DE-" == in)
            return CF::DE_;
        else if ("DEd" == in)
            return CF::DEd;
        else if ("DEm" == in)
            return CF::DEm;
        else if ("DEs" == in)
            return CF::DEs;
        else if ("Vmag" == in)
            return CF::Vmag;
        else
            throw ConfigError("Unknown catalogue description field: " + in);
#undef CF
    }
};
phoenix::function<string_to_field_imp> string_to_field;

template <typename Iterator>
struct catalogue_description_grammar
    : qi::grammar<Iterator, std::vector<CatalogParsingDescription::Entity>(), ascii::space_type>
{
    catalogue_description_grammar()
        : catalogue_description_grammar::base_type(start)
    {
        using boost::phoenix::at_c;

        range = (int_ >> -(lit('-') >> int_))[
            _val = phoenix::construct<std::pair<int, int>>(_1, phoenix::if_else(_2, *_2 - _1 + 1, 1))
            ];
        field = as_string[lexeme[+char_("A-Za-z-")]][_val = string_to_field(_1)];
        element = (range >> field)[
            _val = phoenix::construct<CatalogParsingDescription::Entity>(at_c<0>(_1), at_c<1>(_1), _2)
            ];
        start = element >> *(lit(';') >> element) >> -lit(';');

#ifdef BOOST_SPIRIT_DEBUG
        BOOST_SPIRIT_DEBUG_NODE(start);
        BOOST_SPIRIT_DEBUG_NODE(element);
        BOOST_SPIRIT_DEBUG_NODE(range);
        BOOST_SPIRIT_DEBUG_NODE(field);
#endif
    }

    qi::rule<Iterator, std::vector<CatalogParsingDescription::Entity>(), ascii::space_type> start;
    qi::rule<Iterator, CatalogParsingDescription::Entity(), ascii::space_type> element;
    qi::rule<Iterator, std::pair<int, int>(), ascii::space_type> range;
    qi::rule<Iterator, CatalogParsingDescription::Field(), ascii::space_type> field;
};

CatalogParsingDescription parse_catalogue_description(const std::string & in)
{
    CatalogParsingDescription desc;
    catalogue_description_grammar<std::string::const_iterator> grammar;
    std::string::const_iterator begin(in.begin()), end(in.end());
    bool r(qi::phrase_parse(begin, end, grammar, ascii::space, desc.descriptions));
    if (! r or begin != end)
        throw ConfigError("Parsing catalogue description failed.");
    return desc;
}

}

BOOST_FUSION_ADAPT_ADT(
    config_parser::parser_proxy,
    (const config_variant &, const config_variant &, obj.get(), obj.push_back(val))
)

namespace boost { namespace spirit { namespace traits
{

#ifdef BOOST_SPIRIT_DEBUG
    // SPIRIT_DEBUG doesn't work otherwise

    std::ostream & operator<<(std::ostream & os, const config_variant & v)
    {
        using boost::operator<<;
        os << v;
        return os;
    }

std::ostream & operator<<(std::ostream & os, const CatalogParsingDescription::Entity & e)
{
    os << "Ent: s " << e.start << " l " << e.len << " e " << int(e.field);
    return os;
}

std::ostream & operator<<(std::ostream & os, const CatalogParsingDescription::Field & f)
{
    os << int(f);
    return os;
}

#endif

}}}
