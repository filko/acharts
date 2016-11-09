/*
 * Copyright (c) 2012-2016 Łukasz P. Michalik <lpmichalik@googlemail.com>

 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:

 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef CHART_CATALOGUE_HH
#define CHART_CATALOGUE_HH 1

#include <memory>
#include <string>

class Star;
class Catalogue;
class CatalogParsingDescription;

class ConstStarIterator
{
    friend class Catalogue;

    ConstStarIterator(const Catalogue * c, std::size_t p)
        : cat_(c), index_(p)
    {
    }

public:
    using difference_type = long;
    using value_type = Star;
    using pointer = const value_type*;
    using reference = const value_type&;
    using iterator_category = std::input_iterator_tag;

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

    std::size_t load();

    const ConstStarIterator begin_stars() const;
    const ConstStarIterator end_stars() const;

    double epoch() const;
    void epoch(double epoch);
    void path(const std::string & path);
    const std::string & path() const;
    void mag_limit(double limit);
    double mag_limit() const;
    void description(const CatalogParsingDescription &);
};

#endif
