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
