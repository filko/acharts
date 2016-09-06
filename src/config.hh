#ifndef CHARTS_CONFIG_HH
#define CHARTS_CONFIG_HH 1

#include <memory>
#include <utility>
#include <vector>

#include "catalogue.hh"
#include "projection.hh"
#include "track.hh"

class Config;

class ConstCatalogueIterator
{
    ConstCatalogueIterator(const Config * config, std::size_t index);

    friend class Config;

public:
    ConstCatalogueIterator(const ConstCatalogueIterator & rhs) = default;
    ConstCatalogueIterator & operator=(const ConstCatalogueIterator & rhs) = default;

    ConstCatalogueIterator & operator++();

    Catalogue & operator*() const;

    const std::shared_ptr<Catalogue> operator->() const;

    friend bool operator==(const ConstCatalogueIterator & lhs, const ConstCatalogueIterator & rhs);

private:
    const Config * config_;
    std::size_t index_;
};

inline bool operator!=(const ConstCatalogueIterator & lhs, const ConstCatalogueIterator & rhs)
{
    return !(lhs == rhs);
}

class ConstTrackIterator
{
    ConstTrackIterator(const Config * config, std::size_t index);

    friend class Config;

public:
    ConstTrackIterator(const ConstTrackIterator & rhs) = default;
    ConstTrackIterator & operator=(const ConstTrackIterator & rhs) = default;

    ConstTrackIterator & operator++();

    Track & operator*() const;

    const std::shared_ptr<Track> operator->() const;

    friend bool operator==(const ConstTrackIterator & lhs, const ConstTrackIterator & rhs);

private:
    const Config * config_;
    std::size_t index_;
};

inline bool operator!=(const ConstTrackIterator & lhs, const ConstTrackIterator & rhs)
{
    return !(lhs == rhs);
}

class Config
{
    struct Implementation;
    std::unique_ptr<Implementation> imp_;

    friend class ConstCatalogueIterator;
    friend class ConstTrackIterator;
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

    const ConstCatalogueIterator begin_catalogues() const;
    const ConstCatalogueIterator end_catalogues() const;

    const ConstTrackIterator begin_tracks() const;
    const ConstTrackIterator end_tracks() const;

    const std::vector<std::string> planets() const;
    bool planets_labels() const;

    bool moon() const;
    bool sun() const;
};

#endif
