#include "config.hh"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/variant.hpp>
#include <deque>
#include <fstream>
#include <iostream>

#include "config_parser.hh"
#include "catalogue_description.hh"
#include "exceptions.hh"
#include "now.hh"

struct empty_type {};
std::ostream & operator<<(std::ostream & os, empty_type)
{
    return os;
}

typedef boost::variant<empty_type, angle, timestamp, length, boolean, integer, std::string, double> Option;

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
        ts = config_parser::parse_timestamp(value_);
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

    void operator()(integer & i) const
    {
        i.val = config_parser::parse_integer(value_);
    }

    void operator()(double & d) const
    {
        d = config_parser::parse_double(value_);
    }

    void operator()(empty_type) const
    {
        throw std::runtime_error("Tried to assing to null config tree");
    }
};

Coordinates coordinate_from_string(const std::string & in)
{
    if ("equatorial" == in)
        return Coordinates::Equatorial;
    else if ("horizontal" == in)
        return Coordinates::Horizontal;
    else if ("ecliptic" == in)
        return Coordinates::Ecliptic;

    throw ConfigError("Unsupported coordinates type '" + in + "'.");
}

}

struct Config::Implementation
{
    typedef Option value_type;
    boost::property_tree::basic_ptree<std::string, Option> tree;
    std::string current_section;
    std::deque<std::shared_ptr<Catalogue>> catalogues;
    std::deque<std::shared_ptr<Track>> tracks;
    std::deque<std::shared_ptr<Grid>> grids;
    std::deque<std::shared_ptr<Tick>> ticks;

    Implementation()
        : current_section("core")
    {
        add("core.epoch", timestamp{2451545.0});

        add("core.location.latitude", angle{0.});
        add("core.location.longitude", angle{0.});

        add("core.t", timestamp{Now::get_jd()});

        add("core.output", "output.svg");
        add("core.stylesheet", "");

        add("catalogue.path", "");
        add("catalogue.mag-limit", double{100});
        add("catalogue.pattern", "5-14 Name; 76-77 RAh; 78-79 RAm; 80-83 RAs; 84 DE-; 85-86 DEd; 87-88 DEm; 89-90 DEs; 103-107 Vmag");

        add("canvas.dimensions.x", length{297.});
        add("canvas.dimensions.y", length{210.});
        add("canvas.outer-margin", length{10.});

        add("moon.enable", boolean{false});

        add("projection.type", "AzimuthalEquidistant");

        add("projection.dimensions.ra", angle{45.});
        add("projection.dimensions.dec", angle{0.});
        add("projection.centre.ra", angle{0.});
        add("projection.centre.dec", angle{0.});
        add("projection.level", "none");

        add("planets.enable", "");
        add("planets.labels", boolean{true});

        add("sun.enable", boolean{false});

        add("track.object", "");
        add("track.start", timestamp{Now::get_jd()});
        add("track.length", timestamp{30.});
        add("track.mark-interval", timestamp{7.0});
        add("track.interval-ticks", integer{7});

        add("grid.name", "");
        add("grid.coordinates", "equatorial");
        add("grid.plane", "");
        add("grid.start", angle{0});
        add("grid.end", angle{0});
        add("grid.step", angle{0});
        add("grid.density", angle{0});

        add("tick.name", "");
        add("tick.coordinates", "equatorial");
        add("tick.plane", "");
        add("tick.start", angle{0});
        add("tick.end", angle{0});
        add("tick.step", angle{0});
        add("tick.base", angle{0});
        add("tick.display", "as_degrees");
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
                    catalogues.back()->path(boost::get<std::string>(option));
                else if ("catalogue.mag-limit" == path)
                    catalogues.back()->mag_limit(boost::get<double>(option));
                else if ("catalogue.pattern" == path)
                    catalogues.back()->description(config_parser::parse_catalogue_description(boost::get<std::string>(option)));
                else
                {
                    throw InternalError("Tried setting unknown catalogue property: " + path);
                }
            }
            else if ("track" == current_section)
            {
                Option option(i.data());
                boost::apply_visitor(value_parser_visitor(value), option);
                auto & track(*tracks.back());
                if ("track.object" == path)
                    track.name = boost::get<std::string>(option);
                else if ("track.start" == path)
                    track.start = boost::get<timestamp>(option);
                else if ("track.length" == path)
                    track.length = boost::get<timestamp>(option);
                else if ("track.mark-interval" == path)
                    track.mark_interval = boost::get<timestamp>(option).val();
                else if ("track.interval-ticks" == path)
                    track.interval_ticks = boost::get<integer>(option).val;
                else
                    throw InternalError("Tried setting unknown track property: " + path);
            }
            else if ("grid" == current_section)
            {
                Option option(i.data());
                boost::apply_visitor(value_parser_visitor(value), option);
                auto & grid(*grids.back());
                if ("grid.name" == path)
                    grid.name = boost::get<std::string>(option);
                else if ("grid.coordinates" == path)
                    grid.coordinates = coordinate_from_string(boost::get<std::string>(option));
                else if ("grid.plane" == path)
                {
                    if ("parallel" == boost::get<std::string>(option))
                        grid.plane = Plane::Parallel;
                    else
                        grid.plane = Plane::Meridian;
                }
                else if ("grid.start" == path)
                    grid.start = boost::get<angle>(option);
                else if ("grid.end" == path)
                    grid.end = boost::get<angle>(option);
                else if ("grid.step" == path)
                    grid.step = boost::get<angle>(option);
                else if ("grid.density" == path)
                    grid.density = boost::get<angle>(option);
                else
                    throw ConfigError("Tried setting unknown catalogue property: " + path);
            }
            else if ("tick" == current_section)
            {
                Option option(i.data());
                boost::apply_visitor(value_parser_visitor(value), option);
                auto & tick(*ticks.back());
                if ("tick.name" == path)
                    tick.name = boost::get<std::string>(option);
                else if ("tick.coordinates" == path)
                    tick.coordinates = coordinate_from_string(boost::get<std::string>(option));
                else if ("tick.plane" == path)
                {
                    if ("parallel" == boost::get<std::string>(option))
                        tick.plane = Plane::Parallel;
                    else
                        tick.plane = Plane::Meridian;
                }
                else if ("tick.display" == path)
                {
                    if ("as_hours" == boost::get<std::string>(option))
                        tick.display = Tick::as_hours;
                    else
                        tick.display = Tick::as_degrees;
                }
                else if ("tick.start" == path)
                    tick.start = boost::get<angle>(option);
                else if ("tick.end" == path)
                    tick.end = boost::get<angle>(option);
                else if ("tick.step" == path)
                    tick.step = boost::get<angle>(option);
                else if ("tick.base" == path)
                    tick.base = boost::get<angle>(option);
                else
                    throw ConfigError("Tried setting unknown tick property: " + path);
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
            catalogues.push_back(std::make_shared<Catalogue>());
        else if ("track" == section)
            tracks.push_back(std::make_shared<Track>());
        else if ("grid" == section)
            grids.push_back(std::make_shared<Grid>());
        else if ("tick" == section)
            ticks.push_back(std::make_shared<Tick>());

        current_section = section;
    }

    void dump() const
    {
        for (auto const i : tree)
        {
//            if ("catalogue" == i.first || "track" == i.first)
//                continue;

            std::cout << '[' << i.first << ']' << std::endl;
            dump_section(&i.second);
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
        for (auto const i : *t)
        {
            if (i.second.data().type() != typeid(empty_type))
                std::cout << p << i.first << "\t" << i.second.data() << std::endl;

            dump_section(&i.second, p + i.first + '.');
        }
    }
};

Config::Config(int arc, char * arv[])
    : imp_(new Implementation())
{
    if (arc < 2)
    {
        throw std::runtime_error("No configuration specified on the command line.");
    }
    std::ifstream f(arv[1]);
    if (! f)
        abort();

    config_parser::parse_config(f,
                                [&](const std::string & s)                        { imp_->accept_section(s); },
                                [&](const std::string & p, const std::string & v) { imp_->accept_value(p, v); });
    update_timestamps();
    imp_->dump();
}

Config::~Config()
{
}

timestamp Config::sanitize_timestamp(const timestamp & ts) const
{
    switch (ts.type())
    {
        case timestamp::Type::real:
            return ts;
        case timestamp::Type::t:
            return timestamp{t()};
        case timestamp::Type::now:
            return timestamp{Now::get_jd()};
    }
    throw std::logic_error("Reached end of timestamp::Type switch.");
}

void Config::update_timestamps()
{
    for (auto & track : imp_->tracks)
    {
        track->start = sanitize_timestamp(track->start);
    }
}

const ln_lnlat_posn Config::location() const
{
    return ln_lnlat_posn{imp_->get<angle>("core.location.longitude").val,
                         imp_->get<angle>("core.location.latitude").val};
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

const std::string Config::projection_level() const
{
    return imp_->get<std::string>("projection.level");
}

double Config::t() const
{
    timestamp ts(imp_->get<timestamp>("core.t"));

    switch (ts.type())
    {
        case timestamp::Type::real:
            return ts.val();
        case timestamp::Type::now:
            return Now::get_jd();
        case timestamp::Type::t:
            throw ConfigError("Cannot use 't' when defining core.t");
    }
    throw std::logic_error("Reached end of timestamp::Type switch.");
}

const std::string Config::stylesheet() const
{
    return imp_->get<std::string>("core.stylesheet");
}

const std::string Config::output() const
{
    return imp_->get<std::string>("core.output");
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

template<> const std::shared_ptr<Track> Config::get_collection_item(std::size_t i) const
{
    return imp_->tracks[i];
}

template<> const std::shared_ptr<Catalogue> Config::get_collection_item(std::size_t i) const
{
    return imp_->catalogues[i];
}

template<> const std::shared_ptr<Grid> Config::get_collection_item(std::size_t i) const
{
    return imp_->grids[i];
}

template<> const std::shared_ptr<Tick> Config::get_collection_item(std::size_t i) const
{
    return imp_->ticks[i];
}

template<> const ConfigIterator<Track> Config::View<Track>::end() const
{
    return ConfigIterator<Track>(config_, config_->imp_->tracks.size());
}

template<> const ConfigIterator<Catalogue> Config::View<Catalogue>::end() const
{
    return ConfigIterator<Catalogue>(config_, config_->imp_->catalogues.size());
}

template<> const ConfigIterator<Grid> Config::View<Grid>::end() const
{
    return ConfigIterator<Grid>(config_, config_->imp_->grids.size());
}

template<> const ConfigIterator<Tick> Config::View<Tick>::end() const
{
    return ConfigIterator<Tick>(config_, config_->imp_->ticks.size());
}

template <typename T>
const std::shared_ptr<T> ConfigIterator<T>::operator->() const
{
    return config_->get_collection_item<T>(index_);
}

template const std::shared_ptr<Track> ConfigIterator<Track>::operator->() const;
template const std::shared_ptr<Catalogue> ConfigIterator<Catalogue>::operator->() const;
template const std::shared_ptr<Grid> ConfigIterator<Grid>::operator->() const;
template const std::shared_ptr<Tick> ConfigIterator<Tick>::operator->() const;
