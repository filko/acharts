#ifndef ACHARTS_GZSTREAMBUF_HH
#define ACHARTS_GZSTREAMBUF_HH 1

#include <cassert>
#include <streambuf>
#include <zlib.h>

#include <iostream>
#include <fstream>

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
