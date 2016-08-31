#include "catalogue.hh"

#include <boost/algorithm/string/trim.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "stars.hh"

class CatalogReader
{
    std::istream & stream_;
    typedef std::string Line;
    Line line_;

public:
    CatalogReader(std::istream & stream)
        : stream_(stream)
    {
    }

    bool next()
    {
        return bool(std::getline(stream_, line_));
    }

    std::string get_string(int begin, int size)
    {
        return boost::algorithm::trim_copy(line_.substr(begin, size));
    }

    int get_sign(int begin)
    {
        if ("-" == line_.substr(begin, 1))
            return -1;

        return 1;
    }

    int get_int(int begin, int size)
    {
        std::stringstream s(line_.substr(begin, size));
        int i;
        if (! (s >> i))
            throw std::runtime_error("not an int");
        return i;
    }

    short get_short(int begin, int size)
    {
        std::stringstream s(line_.substr(begin, size));
        short i;
        if (! (s >> i))
            throw std::runtime_error("not a short");
        return i;
    }

    double get_double(int begin, int size)
    {
        std::stringstream s(line_.substr(begin, size));
        double d;
        if (! (s >> d))
            throw std::runtime_error("not a double");
        return d;
    }
};

struct Catalogue::Implementation
{ 
    typedef std::vector<Star> Stars;
    Stars stars_;
    std::string path_;
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

    CatalogReader reader(file);

    while (reader.next())
    {
        try
        {
            // Bright Star Catalogue
            lnh_equ_posn hpos = {
                { reader.get_short(75, 2), reader.get_short(77, 2), reader.get_double(79, 4) },
                { short(reader.get_sign(83) == -1 ? 1 : 0), reader.get_short(84, 2), reader.get_short(86, 2), double(reader.get_short(88, 2)) }
            };

            Star c(reader.get_string(4, 10), hpos, reader.get_double(102, 5));
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
