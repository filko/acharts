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
#ifndef CHART_EXCEPTIONS_HH
#define CHART_EXCEPTIONS_HH

#include <stdexcept>
#include <string>

class ConfigError
    : public std::runtime_error
{
public:
    explicit ConfigError(const std::string & wha)
        : std::runtime_error("Configuration error:\n" + wha)
    {
    }
};

class ConfigValueError
    : public ConfigError
{
public:
    explicit ConfigValueError(const std::string & name, const std::string & value)
        : ConfigError("Cannot parse value.  Expected <" + name + ">, got '" + value + "'.")
    {
    }
};

class InternalError
    : public std::logic_error
{
public:
    explicit InternalError(const std::string & wha)
        : std::logic_error(wha)
    {
    }
};

#endif
