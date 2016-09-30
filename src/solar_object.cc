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
#include "solar_object.hh"

#include <boost/algorithm/string.hpp>
#include <map>

#include "exceptions.hh"
#include "moon_and_sun.hh"
#include "planet.hh"

struct SolarObjectManager::Implementation
{
    std::map<std::string, std::shared_ptr<const SolarObject>> objects;
};

SolarObjectManager::SolarObjectManager()
    : imp_(new Implementation())
{
    const std::string planets[] = { "mercury", "venus", "mars", "jupiter", "saturn", "uranus", "neptune", "pluto"};
    for (auto i(planets), i_end(planets + sizeof(planets) / sizeof(*planets));
         i != i_end; ++i)
        put(std::make_shared<Planet>(*i));

    put(std::make_shared<Sun>());
    put(std::make_shared<Moon>());
}

SolarObjectManager::~SolarObjectManager()
{
}

const std::shared_ptr<const SolarObject> SolarObjectManager::get(const std::string & name) const
{
    auto i(imp_->objects.find(boost::algorithm::to_lower_copy(name)));
    if (imp_->objects.end() != i)
        return i->second;

    throw InternalError("Object '" + name + "' not found");
}

void SolarObjectManager::put(const std::shared_ptr<const SolarObject> & object)
{
    std::string name(boost::algorithm::to_lower_copy(object->name()));
    auto i(imp_->objects.find(name));
    if (imp_->objects.end() != i)
        throw InternalError("Tried adding '" + object->name() + "' but it's already in ObjectManager");

    imp_->objects[name] = object;
}
