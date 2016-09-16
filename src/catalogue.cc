#include "catalogue.hh"

#include <boost/algorithm/string/trim.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "catalogue_description.hh"
#include "stars.hh"

struct Catalogue::Implementation
{ 
    typedef std::vector<Star> Stars;
    Stars stars_;
    std::string path_;
    double mag_limit_ = 100.0;
};

Catalogue::Catalogue()
    : imp_(new Implementation())
{
}

Catalogue::~Catalogue()
{
}

void Catalogue::load()
{
    std::ifstream file(imp_->path_.c_str());
    if (! file)
        throw std::runtime_error("Can't open catalog!");

    std::string line;
    while (std::getline(file, line))
    {
        try
        {
            Star c{parse_line_into_star(descriptions, line)};
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
