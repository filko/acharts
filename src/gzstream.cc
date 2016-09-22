#include "gzstream.hh"

gzstreambuf::gzstreambuf(std::streambuf * sbuf)
    : sbuf_(sbuf), input_(new char[buf_size]), output_(new char[buf_size])
{
    stream_.zalloc = Z_NULL;
    stream_.zfree = Z_NULL;
    stream_.opaque = Z_NULL;
    stream_.avail_in = 0;
    stream_.avail_out = 0;
    stream_.next_in = Z_NULL;
    stream_.next_out = Z_NULL;
    int r(inflateInit2(&stream_, 31));
    if (Z_OK != r)
        throw gzstream_error(stream_.msg);
}

gzstreambuf::~gzstreambuf()
{
    inflateEnd(&stream_);

    delete [] input_;
    delete [] output_;
}

gzstreambuf::int_type gzstreambuf::underflow()
{
    if (0 == stream_.avail_in)
    {
        std::streamsize size{sbuf_->sgetn(input_, buf_size)};
        if (0 == size)
            return traits_type::eof();

        stream_.avail_in = size;
        stream_.next_in = (Bytef *) input_;
    }

    if (0 == egptr() - gptr())
    {
        stream_.avail_out = buf_size;
        stream_.next_out = (Bytef *) output_;
    }

    if (0 != stream_.avail_out)
    {
        int ret{inflate(&stream_, Z_NO_FLUSH)};
        assert(Z_STREAM_ERROR != ret);
        switch (ret)
        {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;
                // keep going
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                return traits_type::eof();
        }
    }

    std::streamsize have{buf_size - stream_.avail_out};
    setg(output_, output_, output_ + have);
    return traits_type::to_int_type(*gptr());
}
