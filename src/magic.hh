#ifndef ACHARTS_MAGIC_HH
#define ACHARTS_MAGIC_HH 1

#include <iosfwd>
#include <memory>

std::unique_ptr<std::istream> open_file_with_magic(const char * path);

#endif
