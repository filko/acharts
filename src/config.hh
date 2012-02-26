#ifndef CHARTS_CONFIG_HH
#define CHARTS_CONFIG_HH 1

#include <memory>
#include <utility>

#include "catalogue.hh"
#include "projection.hh"

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

class Config
{
    struct Implementation;
    std::unique_ptr<Implementation> imp_;

    friend class ConstCatalogueIterator;
public:
    Config(int arc, char * arv[]);
    ~Config();

    std::pair<double, double> location() const;
    const OutputCoord canvas_dimensions() const;
    const std::string projection_type() const;
    const ln_equ_posn projection_centre() const;
    const ln_equ_posn projection_dimensions() const;
    double t() const;

    const ConstCatalogueIterator begin_catalogues() const;
    const ConstCatalogueIterator end_catalogues() const;
};

#endif
