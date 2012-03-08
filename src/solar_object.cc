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
