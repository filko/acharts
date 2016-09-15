#ifndef CHARTS_CONFIG_HH
#define CHARTS_CONFIG_HH 1

#include <memory>
#include <utility>
#include <vector>

#include "catalogue.hh"
#include "projection.hh"
#include "track.hh"

class Config;

template <typename T>
class ConfigIterator
{
    ConfigIterator(const Config * config, std::size_t index)
        : config_(config), index_(index)
    {
    }

    friend class Config;

public:
    ConfigIterator(const ConfigIterator & rhs) = default;
    ConfigIterator & operator=(const ConfigIterator & rhs) = default;

    ConfigIterator & operator++()
    {
        ++index_;
        return *this;
    }

    const std::shared_ptr<T> operator->() const;

    T & operator*() const
    {
        return *(*this).operator->();
    }

    bool operator==(const ConfigIterator & rhs) const
    {
        if (config_ != rhs.config_)
            return false;

        if (index_ != rhs.index_)
            return false;

        return true;
    }

    bool operator!=(const ConfigIterator & rhs) const
    {
        return !(*this == rhs);
    }

private:
    const Config * config_;
    std::size_t index_;
};

class Config
{
    struct Implementation;
    std::unique_ptr<Implementation> imp_;

    template <typename T>
    friend class ConfigIterator;

    template <typename T>
    const std::shared_ptr<T> get_collection_item(std::size_t i) const;

public:
    Config(int arc, char * arv[]);
    ~Config();

    const ln_lnlat_posn location() const;
    const CanvasPoint canvas_dimensions() const;
    double canvas_margin() const;
    const std::string projection_type() const;
    const ln_equ_posn projection_centre() const;
    const ln_equ_posn projection_dimensions() const;
    const std::string projection_level() const;
    double t() const;
    const std::string stylesheet() const;
    const std::string output() const;

    template <typename T>
    struct View;

    template <typename T>
    const View<T> view() const;

    const std::vector<std::string> planets() const;
    bool planets_labels() const;

    bool moon() const;
    bool sun() const;

private:
    timestamp sanitize_timestamp(const timestamp & ts) const;
    void update_timestamps();
};

template <typename T>
struct Config::View
{
    explicit View(const Config * config)
        : config_(config)
    {
    }

    const ConfigIterator<T> begin() const
    {
        return ConfigIterator<T>(config_, 0);
    }
    const ConfigIterator<T> end() const;
    const Config * config_;
};

template <typename T>
const Config::View<T> Config::view() const
{
    return View<T>(this);
}

#endif
