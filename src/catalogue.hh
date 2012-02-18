#ifndef CHART_CATALOGUE_HH
#define CHART_CATALOGUE_HH 1

#include <string>

class Star;
class Catalogue;

class ConstStarIterator
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
    Implementation * imp_;

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
};

#endif
