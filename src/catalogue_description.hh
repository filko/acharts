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
#ifndef ACHARTS_CATALOGUE_DESCRIPTION_HH
#define ACHARTS_CATALOGUE_DESCRIPTION_HH

#include <vector>

#include "stars.hh"

class CatalogParsingDescription
{
public:
    enum class Field
    {
        Name,
        RAh, RAm, RAs,
        DE_, DEd, DEm, DEs,
        Vmag
    };

    struct Entity
    {
        std::size_t start, len;
        Field field;

        Entity() = default;
        Entity(int s, int l, Field f)
            : start(s), len(l), field(f)
        { }
    };
    std::vector<Entity> descriptions;
};

const Star parse_line_into_star(const CatalogParsingDescription & description, const std::string & line);

extern CatalogParsingDescription descriptions;

#endif
