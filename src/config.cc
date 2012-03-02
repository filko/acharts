#include "config.hh"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/variant.hpp>
#include <deque>
#include <fstream>

#include "catalogue.hh"
#include "config_parser.hh"
#include "exceptions.hh"
#include "now.hh"

struct angle
{
    double val;
};

std::ostream & operator<<(std::ostream & os, const angle & a)
{
    os << "Angle: " << a.val;
    return os;
}

struct timestamp
{
    double val;
};

std::ostream & operator<<(std::ostream & os, const timestamp & t)
{
    auto flags(os.flags());
    os << "Timestamp: " << std::fixed << t.val;
    os.flags(flags);
    return os;
}

struct length
{
    double val;
};

std::ostream & operator<<(std::ostream & os, const length & s)
{
    os << "Length: " << s.val << " mm";
    return os;
}

struct boolean
{
    bool val;
};

std::ostream & operator<<(std::ostream & os, const boolean & b)
{
    os << (b.val ? "on" : "off");
    return os;
}

struct empty_type {};
std::ostream & operator<<(std::ostream & os, empty_type)
{
    return os;
}

typedef boost::variant<empty_type, angle, timestamp, length, boolean, std::string> Option;

namespace
{
struct value_parser_visitor
    : boost::static_visitor<>
{
    const std::string & value_;
    value_parser_visitor(const std::string & value)
        : value_(value)
    {
    }

    void operator()(angle & a) const
    {
        a.val = config_parser::parse_angle(value_);
    }

    void operator()(timestamp & ts) const
    {
        ts.val = config_parser::parse_timestamp(value_);
    }

    void operator()(std::string & s) const
    {
        s = value_;
    }

    void operator()(length & s) const
    {
        s.val = config_parser::parse_length(value_);
    }

    void operator()(boolean & b) const
    {
        b.val = config_parser::parse_boolean(value_);
    }

    void operator()(empty_type) const
    {
        throw std::runtime_error("Tried to assing to null config tree");
    }
};

}

struct Config::Implementation
{
    typedef Option value_type;
    boost::property_tree::basic_ptree<std::string, Option> tree;
    std::string current_section;
    std::deque<std::shared_ptr<Catalogue>> catalogues;

    Implementation()
        : current_section("core")
    {
        add("core.epoch", timestamp{0.});

        add("core.location.latitude", angle{0.});
        add("core.location.longitude", angle{0.});

        add("core.t", timestamp{Now::get_jd()});

        add("catalogue.path", "");

        add("canvas.dimensions.x", length{297.});
        add("canvas.dimensions.y", length{210.});
        add("canvas.outer-margin", length{10.});

        add("moon.enable", boolean{false});

        add("projection.type", "AzimuthalEquidistant");

        add("projection.dimensions.ra", angle{45.});
        add("projection.dimensions.dec", angle{0.});
        add("projection.centre.ra", angle{0.});
        add("projection.centre.dec", angle{0.});

        add("planets.enable", "");
        add("planets.labels", boolean{true});

        add("sun.enable", boolean{false});
    }

    void accept_value(const std::string & path, const std::string & value)
    {
        try
        {
            auto & i(tree.get_child(path));

            if ("catalogue" == current_section)
            {
                Option option(i.data());
                boost::apply_visitor(value_parser_visitor(value), option);
                if ("catalogue.path" == path)
                {
                    catalogues.back()->path(boost::get<std::string>(option));
                }
                else
                {
                    throw InternalError("Tried setting unknown catalogue property: " + path);
                }
            }
            else
                boost::apply_visitor(value_parser_visitor(value), i.data());
        }
        catch (const boost::property_tree::ptree_bad_path &)
        {
            throw ConfigError("Tried setting unknown option '" + path + "'");
        }
    }

    void accept_section(const std::string & section)
    {
        try
        {
            tree.get_child(section);
        }
        catch (const boost::property_tree::ptree_bad_path &)
        {
            throw ConfigError("Tried opening unknown section '" + section + "'");
        }
        if ("catalogue" == section)
        {
            catalogues.push_back(std::make_shared<Catalogue>());
        }
        current_section = section;
    }

    void dump() const
    {
        for (auto i(tree.begin()), i_end(tree.end());
             i != i_end; ++i)
        {
            if ("catalogue" == i->first)
                continue;

            std::cout << '[' << i->first << ']' << std::endl;
            dump_section(&i->second);
        }
    }

    template <typename T>
    void add(const std::string & path, const T & t)
    {
        tree.put(path, Option(t));
    }

    template <typename T>
    T get(const std::string & path)
    {
        const auto & i(tree.get_child(path));
        return boost::get<T>(i.data());
    }

private:
    void dump_section(const decltype(tree) * t, const std::string & p = "") const
    {
        for (auto i(t->begin()), i_end(t->end());
             i != i_end; ++i)
        {
            if (i->second.data().type() != typeid(empty_type))
                std::cout << p << i->first << "\t" << i->second.data() << std::endl;

            dump_section(&i->second, p + i->first + '.');
        }
    }
};

Config::Config(int arc, char * arv[])
    : imp_(new Implementation())
{
    if (arc < 2)
    {
        abort();
    }
    std::ifstream f(arv[1]);
    if (! f)
        abort();

    config_parser::parse_config(f,
                                [&](const std::string & s)                        { imp_->accept_section(s); },
                                [&](const std::string & p, const std::string & v) { imp_->accept_value(p, v); });

    imp_->dump();
}

Config::~Config()
{
}

std::pair<double, double> Config::location() const
{
    return std::make_pair(imp_->get<angle>("core.location.latitude").val,
                          imp_->get<angle>("core.location.longitude").val);
}

const CanvasPoint Config::canvas_dimensions() const
{
    return CanvasPoint(imp_->get<length>("canvas.dimensions.x").val,
                       imp_->get<length>("canvas.dimensions.y").val);
}

double Config::canvas_margin() const
{
    return imp_->get<length>("canvas.outer-margin").val;
}

bool Config::moon() const
{
    return imp_->get<boolean>("moon.enable").val;
}

bool Config::sun() const
{
    return imp_->get<boolean>("sun.enable").val;
}

const std::string Config::projection_type() const
{
    return imp_->get<std::string>("projection.type");
}

const ln_equ_posn Config::projection_centre() const
{
    return ln_equ_posn{imp_->get<angle>("projection.centre.ra").val,
                       imp_->get<angle>("projection.centre.dec").val};
}

const ln_equ_posn Config::projection_dimensions() const
{
    return ln_equ_posn{imp_->get<angle>("projection.dimensions.ra").val,
                       imp_->get<angle>("projection.dimensions.dec").val};
}

double Config::t() const
{
    return imp_->get<timestamp>("core.t").val;
}

const std::vector<std::string> Config::planets() const
{
    std::string v(imp_->get<std::string>("planets.enable"));
    if ("all" == v)
        v = "Mercury,Venus,Mars,Jupiter,Saturn,Uranus,Neptune,Pluto";

    std::vector<std::string> ret;
    boost::algorithm::split(ret, v, boost::algorithm::is_any_of(","), boost::algorithm::token_compress_on);

    for (auto i(ret.begin()); i != ret.end(); )
    {
        if (i->empty())
            i = ret.erase(i);
        else
        {
            boost::algorithm::trim(*i);
            boost::algorithm::to_lower(*i);
            ++i;
        }
    }
    return ret;
}

bool Config::planets_labels() const
{
    return imp_->get<boolean>("planets.labels").val;
}

const ConstCatalogueIterator Config::begin_catalogues() const
{
    return ConstCatalogueIterator(this, 0);
}
    
const ConstCatalogueIterator Config::end_catalogues() const
{
    return ConstCatalogueIterator(this, imp_->catalogues.size());
}

ConstCatalogueIterator::ConstCatalogueIterator(const Config * config, std::size_t i)
    : config_(config),
      index_(i)
{
}

ConstCatalogueIterator & ConstCatalogueIterator::operator++()
{
    ++index_;
    return *this;
}

Catalogue & ConstCatalogueIterator::operator*() const
{
    return *config_->imp_->catalogues[index_];
}

const std::shared_ptr<Catalogue> ConstCatalogueIterator::operator->() const
{
    return config_->imp_->catalogues[index_];
}

bool operator==(const ConstCatalogueIterator & lhs, const ConstCatalogueIterator & rhs)
{
    if (lhs.config_ != rhs.config_)
        return false;

    if (lhs.index_ != rhs.index_)
        return false;

    return true;
}
