#ifndef ACHARTS_SVG_PAINTER_HH_
#define ACHARTS_SVG_PAINTER_HH_ 1

#include <boost/variant/static_visitor.hpp>
#include <memory>

#include "scene_storage.hh"

class SvgPainter
    : public boost::static_visitor<>
{
    struct Implementation;
    std::unique_ptr<Implementation> imp_;
    std::ostream & os_;
    const scene::Group * parent_;

public:
    SvgPainter(std::ostream & os, const CanvasPoint & canvas, double canvas_margin, const std::string & style);
    ~SvgPainter();
    SvgPainter(const SvgPainter &) = delete;
    
    void operator()(const scene::Object & o);
    void operator()(const scene::Group & g);
    void operator()(const scene::Rectangle & r);
    void operator()(const scene::Line & l);
    void operator()(const scene::Path & p);
    void operator()(const scene::Text & t);
};

#endif
