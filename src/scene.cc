#include "scene.hh"

namespace scene
{

Scene::Scene(const std::shared_ptr<Projection> & projection)
    : projection_(projection),
      scene_{"", "root", {}}
{
}

void Scene::add_group(Group && group)
{
    scene_.elements.push_back(std::move(group));
}

}
