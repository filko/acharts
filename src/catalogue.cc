#include "catalogue.hh"

#include <boost/algorithm/string/trim.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>

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
        return std::getline(stream_, line_);
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

void Catalogue::load()
{
    std::ifstream file(path_.c_str());
    if (! file)
        throw std::runtime_error("Can't open catalog!");

    CatalogReader reader(file);

    while (reader.next())
    {
        try
        {
            lnh_equ_posn hpos = {
                { reader.get_short(75, 2), reader.get_short(77, 2), reader.get_double(79, 4) },
                { short(reader.get_sign(83) == -1 ? 1 : 0), reader.get_short(84, 2), reader.get_short(86, 2), double(reader.get_short(88, 2)) }
            };

            Star c(reader.get_string(4, 10), hpos, reader.get_double(102, 5));
            stars_.push_back(c);
        }
        catch (const std::runtime_error &)
        {
            /* skippery */
        }
    }

    std::sort(stars_.begin(), stars_.end(), Star::by_mag());
}
