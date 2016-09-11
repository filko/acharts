#include "scene_storage.hh"

namespace scene
{

Storage::Storage(const std::shared_ptr<Projection> & projection)
    : projection_(projection),
      scene_{"", "root", {}}
{
}

void Storage::add_group(Group && group)
{
    scene_.elements.push_back(std::move(group));
}

}
