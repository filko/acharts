#ifndef ACHARTS_SCENE_STORAGE_HH_
#define ACHARTS_SCENE_STORAGE_HH_ 1

#include <boost/variant/recursive_variant.hpp>
#include <deque>
#include <libnova/ln_types.h>

#include "bezier.hh"
#include "projection.hh"

namespace scene
{

struct Object
{
    CanvasPoint pos;
    double mag;
};

struct Rectangle
{
    CanvasPoint start, size;
};

struct Line
{
    CanvasPoint start, end;
};

struct Path
{
    BezierCurve path;
};

struct Text
{
    std::string body;
    CanvasPoint pos;
};

struct Group;

typedef boost::variant<
    boost::recursive_wrapper<Group>,
    Object,
    Rectangle,
    Line,
    Path,
    Text
    > Element;

struct Group
{
    std::string class_;
    std::string id;
    std::deque<Element> elements;

    Group(const Group &) = delete;
    Group(Group &&) = default;
};

class Storage
{
    const std::shared_ptr<Projection> projection_;
    Group scene_;

public:
    Storage(const std::shared_ptr<Projection> & projection);
    Storage(const Storage &) = delete;

    void add_group(Group && group);

    template <typename Visitor>
    void apply_visitor(Visitor & v)
    {
        for (auto const & el : scene_.elements)
            boost::apply_visitor(v, el);
    }
};

}

#endif