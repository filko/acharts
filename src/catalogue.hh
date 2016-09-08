#ifndef CHART_CATALOGUE_HH
#define CHART_CATALOGUE_HH 1

#include <memory>
#include <string>

class Star;
class Catalogue;

class ConstStarIterator
    : public std::iterator<std::input_iterator_tag, Star>
{
    friend class Catalogue;

    ConstStarIterator(const Catalogue * c, std::size_t p)
        : cat_(c), index_(p)
    {
    }

public:
    ~ConstStarIterator() = default;

    const Star * operator->() const;
    const Star & operator*() const;
    ConstStarIterator & operator++() { ++index_; return *this; }

    bool operator==(const ConstStarIterator & rhs) const;
    bool operator!=(const ConstStarIterator & rhs) const { return !(*this == rhs); }

private:
    const Catalogue * cat_;
    std::size_t index_;
};

class Catalogue
{
    struct Implementation;
    std::unique_ptr<Implementation> imp_;

    friend class ConstStarIterator;

public:
    Catalogue();
    ~Catalogue();
    Catalogue(const Catalogue &) = delete;
    Catalogue & operator=(const Catalogue &) = delete;

    void load();

    const ConstStarIterator begin_stars() const;
    const ConstStarIterator end_stars() const;

    void path(const std::string & path);
    const std::string & path() const;
    void mag_limit(double limit);
    double mag_limit() const;
};

#endif
