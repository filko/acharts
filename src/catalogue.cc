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
