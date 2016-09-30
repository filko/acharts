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
#ifndef CHART_CONFIG_PARSER_HH

#include <istream>
#include <functional>

#include "types.hh"

class CatalogParsingDescription;

namespace config_parser
{

typedef std::function<void (const std::string &, const std::string &)> ValueCallback;
typedef std::function<void (const std::string &)> SectionCallback;

void parse_config(std::istream & os, const SectionCallback & scal, const ValueCallback & vcal);
double parse_angle(const std::string & in);
timestamp parse_timestamp(const std::string & in);
double parse_length(const std::string & in);
bool parse_boolean(const std::string & in);
int parse_integer(const std::string & in);
double parse_double(const std::string & in);

CatalogParsingDescription parse_catalogue_description(const std::string & in);

}

#endif
