#include "config.hh"

#include <boost/property_tree/ptree.hpp>
#include <boost/variant.hpp>
#include <fstream>

#include "config_parser.hh"
#include "exceptions.hh"

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

struct empty_type {};
std::ostream & operator<<(std::ostream & os, empty_type)
{
    return os;
}

typedef boost::variant<empty_type, angle, timestamp> Option;

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

    Implementation()
    {
        add("core.epoch", timestamp{0.});

        add("core.location.latitude", angle{0.});
        add("core.location.longitude", angle{0.});
    }

    void accept(const std::string & path, const std::string & value)
    {
        try
        {
            auto & i(tree.get_child(path));
            boost::apply_visitor(value_parser_visitor(value), i.data());
        }
        catch (const boost::property_tree::ptree_bad_path &)
        {
            throw std::runtime_error("Tried setting unknown option '" + path + "'");
        }
    }

    void dump() const
    {
        for (auto i(tree.begin()), i_end(tree.end());
             i != i_end; ++i)
        {
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

    config_parser::parse_config(f, [&](const std::string & p, const std::string & v) { imp_->accept(p, v); });

    imp_->dump();
}

Config::~Config()
{
    delete imp_;
}

std::pair<double, double> Config::location() const
{
    return std::make_pair(imp_->get<angle>("core.location.latitude").val,
                          imp_->get<angle>("core.location.longitude").val);
}
