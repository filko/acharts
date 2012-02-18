#ifndef CHART_CATALOGUE_HH
#define CHART_CATALOGUE_HH 1

#include <string>

#include "stars.hh"

class Catalogue
{
public:
    typedef std::vector<Star> Stars;

    Catalogue() = default;
    Catalogue(const Catalogue &) = delete;
    Catalogue & operator=(const Catalogue &) = delete;

    void load();

    std::size_t size() const            { return stars_.size(); }

    const Stars & get_stars() const     { return stars_; }

    void path(const std::string & path) { path_ = path; }
    const std::string & path() const                   { return path_; }

private:
    Stars stars_;
    std::string path_;
};

#endif
