#include "magic.hh"

#include <fstream>

#include "gzstream.hh"

namespace
{

const std::array<char, 2> gzip_magic = {{ 0x1f, char(0x8b) }};

}

std::unique_ptr<std::istream> open_file_with_magic(const char * path)
{
    std::array<char, 2> signature;
    std::ifstream f(path);
    if (2 != f.rdbuf()->sgetn(&signature[0], 2))
        throw std::runtime_error(std::string("Can't open catalogue at ") + path);

    if (gzip_magic == signature)
    {
        return std::unique_ptr<std::istream>(new igzfstream(path));
    }

    return std::unique_ptr<std::istream>(new std::ifstream(path));
}
