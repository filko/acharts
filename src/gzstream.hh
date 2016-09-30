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
#ifndef ACHARTS_GZSTREAMBUF_HH
#define ACHARTS_GZSTREAMBUF_HH 1

#include <cassert>
#include <streambuf>
#include <zlib.h>

#include <iostream>
#include <fstream>
#include <stdexcept>

class gzstream_error
    : public std::runtime_error
{
public:
    gzstream_error(const std::string & s)
        : std::runtime_error(s)
    {
    }
};

class gzstreambuf
    : public std::streambuf
{
    static const std::streamsize buf_size{128 * 1024};

    std::streambuf * sbuf_;
    z_stream stream_;
    char * input_;
    char * output_;
public:
    explicit gzstreambuf(std::streambuf * sbuf);
    gzstreambuf(const gzstreambuf &) = delete;
    gzstreambuf(gzstreambuf &&) = delete;

    ~gzstreambuf();

    int_type underflow() override;
};

class igzfstream
    : public std::istream
{
private:
    std::filebuf filebuf_;
    gzstreambuf gzstreambuf_;

public:
    igzfstream(const char * path)
        : std::istream(), filebuf_(), gzstreambuf_(&filebuf_)
    {
        filebuf_.open(path, std::ios_base::in);
        this->init(&gzstreambuf_);
    }
};

#endif
