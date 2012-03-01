#include "drawer.hh"

#include <cmath>
#include <cstring>
#include <deque>
#include <fstream>
#include <stdexcept>

#include "bezier.hh"

namespace
{

const std::string svg_preambule(
    "<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n"
    "<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN' 'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>\n"
    );

struct svg_shape
{
    svg_shape() = default;
    svg_shape(const svg_shape &) = delete;
    svg_shape & operator=(const svg_shape &) = delete;

    virtual void flush(std::ostream & out) = 0;

    virtual ~svg_shape()
    {
    }
};

struct svg_circle
    : public svg_shape
{
    CanvasPoint pos;
    double radius;
    std::string fill;
    double stroke_width;
    std::string stroke;

    svg_circle(const CanvasPoint & pos, double radius, const std::string & fill,
               double stroke_width, const std::string & stroke)
        : pos(pos), radius(radius), fill(fill),
          stroke_width(stroke_width), stroke(stroke)
    {
    }

    virtual void flush(std::ostream & out)
    {
        out << "<circle cx='" << pos.x << "' cy='" << pos.y << "' r='" << radius << "' fill='" << fill << "' "
            "stroke='" << stroke << "' stroke-width='" << stroke_width << "' />\n";
    }
};

struct svg_rect
    : public svg_shape
{
    CanvasPoint start, size;
    std::string fill;

    svg_rect(const CanvasPoint & start, const CanvasPoint & size, const std::string & fill)
        : start(start), size(size), fill(fill)
    {
    }

    virtual void flush(std::ostream & out)
    {
        out << "<rect x='" << start.x << "' y='" << start.y << "' "
            "width='" << size.x << "' height='" << size.y << "' fill='" << fill << "' />\n";
    }
};

struct svg_cbezier
    : public svg_shape
{
    std::vector<CanvasPoint> path;
    std::string stroke;
    double width;

    svg_cbezier(std::vector<CanvasPoint> && path, const std::string & stroke, double width)
        : path(std::move(path)), stroke(stroke), width(width)
    {
    }

    virtual void flush(std::ostream & out)
    {
        auto beziered(interpolate_bezier(path));
/*
        // drawing ugly control points, usefull for debugging
        for (auto i(beziered.cbegin()), i_end(beziered.cend());
             i != i_end; ++i)
        {
            out << "<circle cx='" << i->p.x << "' cy='" << i->p.y << "' r='2px' fill='black' />\n";
            out << "<circle cx='" << i->cm.x << "' cy='" << i->cm.y << "' r='1px' fill='blue' />\n";
            out << "<circle cx='" << i->cp.x << "' cy='" << i->cp.y << "' r='1px' fill='green' />\n";
        }
*/

        out << "<path stroke='gray' stroke-width='" << width << "' fill='none'\n  d='";
        auto prev(beziered.cbegin());
        out << 'M' << prev->p.x << ',' << prev->p.y << ' ';
        for (auto next(prev + 1), i_end(beziered.cend());
             next != i_end; prev = next, ++next)
        {
            out << 'C' << prev->cp.x << ',' << prev->cp.y << ' '
                << next->cm.x << ',' << next->cm.y << ' '
                << next->p.x << ',' << next->p.y << ' ';
        }
        out << "' />\n";
    }
};

struct svg_text
    : public svg_shape
{
    std::string body;
    CanvasPoint pos;
    double size;

    svg_text(const std::string & body, const CanvasPoint & pos, double size)
        : body(body), pos(pos), size(size)
    {
    }

    virtual void flush(std::ostream & out)
    {
        out << "<text x='" << pos.x << "' y='" << pos.y << "' font-size='" << size << "' font-family='sans-serif'>\n"
            << body << "\n</text>\n";
    }
};

}

struct Drawer::Implementation
{
    std::shared_ptr<Projection> projection_;
    const CanvasPoint canvas_, canvas_start_;
    std::deque<std::shared_ptr<svg_shape>> shapes_;

    Implementation(const CanvasPoint & canvas, const CanvasPoint & scanvas)
        : canvas_(canvas), canvas_start_(scanvas)
    {
    }

    bool in_canvas(const CanvasPoint & oc)
    {
        return oc.x >= canvas_start_.x && oc.y >= canvas_start_.y &&
            oc.x <= canvas_.x && oc.x <= canvas_.y;
    }

    double mag2size(double mag)
    {
        static const double e(exp(1));
        return exp(-mag / e);
    }

    double draw_by_magnitudo(const SolarObject & object, double jd, const CanvasPoint & coord)
    {
        double s(mag2size(object.get_magnitude(jd)));
        shapes_.push_back(std::make_shared<svg_circle>(coord, s, "red", s/10., "blue"));
        return s;
    }

    double draw_by_sdiam(const SolarObject & object, double jd, const CanvasPoint & coord, const ln_equ_posn & pos)
    {
        double s(object.get_sdiam(jd) * projection_->scale_at_point(pos) / 3600.);
        shapes_.push_back(std::make_shared<svg_circle>(coord, s, "#22222", 0., ""));
        return s;
    }
};

Drawer::Drawer(const CanvasPoint & canvas)
    : imp_(new Implementation(canvas, CanvasPoint(- canvas.x / 2., - canvas.y / 2.)))
{
    std::string background_color("white");
    imp_->shapes_.push_back(
        std::make_shared<svg_rect>(CanvasPoint(imp_->canvas_start_.x, imp_->canvas_start_.y),
                                   CanvasPoint(imp_->canvas_.x, imp_->canvas_.y), background_color));
}

Drawer::~Drawer()
{
}

void Drawer::set_projection(const std::shared_ptr<Projection> & projection)
{
    imp_->projection_ = projection;
}

void Drawer::store(const char * file) const
{
    std::ofstream of(file);
    if (! of)
        throw std::runtime_error("Can't open file '" + std::string(file) + "' for writing " + std::strerror(errno));

    of << svg_preambule;

    of << "<svg width='" << imp_->canvas_.x << "mm' height='" << imp_->canvas_.y << "mm' "
        "viewBox='" << imp_->canvas_start_.x << ' ' << imp_->canvas_start_.y <<
        ' ' << imp_->canvas_.x << ' ' << imp_->canvas_.y << "' "
        "xmlns='http://www.w3.org/2000/svg' version='1.1'>\n";

    for (auto shape(imp_->shapes_.begin()), shape_end(imp_->shapes_.end());
         shape != shape_end; ++shape)
        (*shape)->flush(of);

    of << "</svg>\n";
}

void Drawer::draw(const Star & star)
{
    static std::string black("black");
    static std::string white("white");

    CanvasPoint coord(imp_->projection_->project(star.pos_));
    if (! imp_->in_canvas(coord))
        return;

    double s(imp_->mag2size(star.vmag_));
    auto c(std::make_shared<svg_circle>(coord, s, black, .5 * s, white));
    imp_->shapes_.push_back(c);

//    if (star.vmag_ < 4. && ! star.common_name_.empty())
//        imp_->board << Lb::Text(-star.pos_.ra + s, star.pos_.dec, star.common_name_, Lb::Fonts::Helvetica, 3.);
}

void Drawer::draw(const std::vector<ln_equ_posn> & path)
{
    std::vector<CanvasPoint> ret;
    for (auto i(path.begin()), i_end(path.end());
         i != i_end; ++i)
        ret.push_back(imp_->projection_->project(*i));
    imp_->shapes_.push_back(std::make_shared<svg_cbezier>(std::move(ret), "#888888", 0.1));
}

void Drawer::draw(const std::string & body, const ln_equ_posn & pos)
{
    imp_->shapes_.push_back(std::make_shared<svg_text>(body, imp_->projection_->project(pos), 4.));
}

void Drawer::draw(const SolarObject & object, double jd, object_rendering_type type, bool label)
{
    auto pos(object.get_equ_coords(jd));
    auto coord(imp_->projection_->project(pos));

    double s(0.);
    switch (type)
    {
        case magnitudo:
            s = imp_->draw_by_magnitudo(object, jd, coord);
            break;
        case sdiam:
            s = imp_->draw_by_sdiam(object, jd, coord, pos);
            break;
    }

    if (label)
    {
        coord.x += s;
        coord.y += 1.;
        imp_->shapes_.push_back(std::make_shared<svg_text>(object.name(), coord, 4.));
    }
}
