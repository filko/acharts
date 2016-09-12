#include "svg_painter.hh"

#include <boost/variant/apply_visitor.hpp>

namespace
{

double mag2size(double mag)
{
    static const double e(exp(1));
    return 2.0 * exp(-mag / e) + 0.1;
}

}

struct SvgPainter::Implementation
{
    const double canvas_margin_;
    const CanvasPoint canvas_, canvas_start_, canvas_end_;
    const std::string style_;

    Implementation(const CanvasPoint & canvas, double canvas_margin, const std::string & style)
        : canvas_margin_(canvas_margin),
          canvas_(canvas),
          canvas_start_(-canvas.x / 2. - canvas_margin_, -canvas.y / 2. - canvas_margin_),
          canvas_end_(canvas.x / 2. + canvas_margin_, canvas_.y / 2. + canvas_margin_),
          style_(style)
    {
    }

    bool in_canvas(const CanvasPoint & oc)
    {
        return oc.x >= canvas_start_.x && oc.y >= canvas_start_.y &&
            oc.x <= canvas_end_.x && oc.y <= canvas_end_.y;
    }
};

SvgPainter::SvgPainter(std::ostream & os, const CanvasPoint & canvas, double canvas_margin, const std::string & style)
    : imp_(new Implementation(canvas, canvas_margin, style)),
      os_(os)
{
    os_ << "<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n"
        "<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN' 'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>\n"
        "<svg width='" << imp_->canvas_.x << "mm' height='" << imp_->canvas_.y << "mm' "
        "viewBox='" << -imp_->canvas_.x / 2. << ' ' << -imp_->canvas_.y / 2. <<
        ' ' << imp_->canvas_.x << ' ' << imp_->canvas_.y << "' "
        "xmlns='http://www.w3.org/2000/svg' version='1.1'>\n"
        "<style type='text/css'>\n" << imp_->style_ << "</style>\n";
}

SvgPainter::~SvgPainter()
{
    os_ << "</svg>\n";
}

void SvgPainter::operator()(const scene::Group & g)
{
    parent_ = & g;

    os_ << "<g class='" << g.class_ << "' id='" << g.id << "'>\n";
    for_each(g.elements.begin(), g.elements.end(),
             boost::apply_visitor(*this));
    os_ << "</g>\n";
}

void SvgPainter::operator()(const scene::Object & o)
{
    if (!imp_->in_canvas(o.pos))
        return;

    double radius{mag2size(o.mag)},
        stroke_width{0.2 * radius};
    os_ << "<circle cx='" << o.pos.x << "' cy='" << o.pos.y << "' "
        "r='" << radius << "' stroke-width='" << stroke_width << "' />\n";
}

void SvgPainter::operator()(const scene::Rectangle & r)
{
    os_ << "<rect x='" << r.start.x << "' y='" << r.start.y << "' "
        "width='" << r.size.x << "' height='" << r.size.y << "'/>\n";
}

void SvgPainter::operator()(const scene::Line &)
{
    throw std::runtime_error("svgPainter for line not defined!");
}

void SvgPainter::operator()(const scene::Path & p)
{
#if 0
    // drawing ugly control points, usefull for debugging
    for (auto const & i : p.path)
    {
        os_ << "<circle cx='" << i.p.x << "' cy='" << i.p.y << "' r='2px' fill='black' />\n";
        os_ << "<circle cx='" << i.cm.x << "' cy='" << i.cm.y << "' r='1px' fill='blue' />\n";
        os_ << "<circle cx='" << i.cp.x << "' cy='" << i.cp.y << "' r='1px' fill='green' />\n";
    }
#endif

    os_ << "<path d='";
    auto prev(p.path.cbegin());
    os_ << 'M' << prev->p.x << ',' << prev->p.y << ' ';
    for (auto next(prev + 1), i_end(p.path.cend());
         next != i_end; prev = next, ++next)
    {
        os_ << 'C' << prev->cp.x << ',' << prev->cp.y << ' '
            << next->cm.x << ',' << next->cm.y << ' '
            << next->p.x << ',' << next->p.y << ' ';
    }
    os_ << "' />\n";
}

void SvgPainter::operator()(const scene::Text & t)
{
    os_ << "<text x='" << t.pos.x << "' y='" << t.pos.y << "'>" << t.body << "</text>\n";
}
