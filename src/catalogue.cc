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
#include "catalogue.hh"

#include <boost/algorithm/string/trim.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "catalogue_description.hh"
#include "config_parser.hh"
#include "magic.hh"
#include "stars.hh"

struct Catalogue::Implementation
{
    typedef std::vector<Star> Stars;
    Stars stars_;
    double epoch_ = 2451545.0;
    std::string path_;
    double mag_limit_ = 100.0;
    CatalogParsingDescription description;

    Implementation()
        : description(config_parser::parse_catalogue_description(
                          "5-14 Name; 76-77 RAh; 78-79 RAm; 80-83 RAs; 84 DE-; 85-86 DEd; 87-88 DEm; 89-90 DEs; 103-107 Vmag"))
    { }
};

Catalogue::Catalogue()
    : imp_(new Implementation())
{
}

Catalogue::~Catalogue()
{
}

std::size_t Catalogue::load()
{
    std::unique_ptr<std::istream> file(open_file_with_magic(imp_->path_.c_str()));
    if (! file)
        throw std::runtime_error("Can't open catalog!");

    std::string line;
    std::size_t count{0};
    while (std::getline(*file, line))
    {
        try
        {
            Star c{parse_line_into_star(imp_->description, line)};
            ++count;
            if (c.vmag_ > imp_->mag_limit_)
                continue;

            imp_->stars_.push_back(c);
        }
        catch (const std::runtime_error &)
        {
            /* skippery */
        }
    }

    std::sort(imp_->stars_.begin(), imp_->stars_.end(), Star::by_mag());
    return count;
}

const ConstStarIterator Catalogue::begin_stars() const
{
    return ConstStarIterator(this, 0);
}

const ConstStarIterator Catalogue::end_stars() const
{
    return ConstStarIterator(this, imp_->stars_.size());
}

double Catalogue::epoch() const
{
    return imp_->epoch_;
}

void Catalogue::epoch(double epoch)
{
    imp_->epoch_ = epoch;
}

void Catalogue::path(const std::string & path)
{
    imp_->path_ = path;
}

const std::string & Catalogue::path() const
{
    return imp_->path_;
}

void Catalogue::mag_limit(double limit)
{
    imp_->mag_limit_ = limit;
}

double Catalogue::mag_limit() const
{
    return imp_->mag_limit_;
}

void Catalogue::description(const CatalogParsingDescription & description)
{
    imp_->description = description;
}

const Star & ConstStarIterator::operator*() const
{
    return cat_->imp_->stars_[index_];
}

const Star * ConstStarIterator::operator->() const
{
    return &(**this);
}

bool ConstStarIterator::operator==(const ConstStarIterator & rhs) const
{
    if (cat_ != rhs.cat_)
        return false;

    return index_ == rhs.index_;
}
