#include "drawer.hh"

#include <cmath>
#include <cstring>
#include <deque>
#include <fstream>
#include <stdexcept>

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
    OutputCoord pos;
    double radius;
    std::string fill;
    double stroke_width;
    std::string stroke;

    svg_circle(const OutputCoord & pos, double radius, const std::string & fill,
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
    OutputCoord start, size;
    std::string fill;

    svg_rect(const OutputCoord & start, const OutputCoord & size, const std::string & fill)
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
    std::vector<OutputCoord> path;
    std::string stroke;
    double width;

    svg_cbezier(std::vector<OutputCoord> && path, const std::string & stroke, double width)
        : path(std::move(path)), stroke(stroke), width(width)
    {
    }

    virtual void flush(std::ostream & out)
    {
        if (! path.empty())
        {
            out << "<path fill='none' stroke='" << stroke << "' stroke-width='" << width << "' "
                "d='M";
            auto i(path.begin()), i_end(path.end());
            out << i->x << ',' << i->y << ' ';
            ++i;
            while (i != i_end)
            {
                out << "L" << i->x << ',' << i->y << ' ';
                ++i;
            }

            out << "' />\n";
        }
    }
};

struct svg_text
    : public svg_shape
{
    std::string body;
    OutputCoord pos;
    double size;

    svg_text(const std::string & body, const OutputCoord & pos, double size)
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

struct Drawer::Impl
{
    std::shared_ptr<Projection> projection_;
    const OutputCoord canvas_, canvas_start_;
    std::deque<std::shared_ptr<svg_shape>> shapes_;

    Impl(const OutputCoord & canvas, const OutputCoord & scanvas)
        : canvas_(canvas), canvas_start_(scanvas)
    {
    }

    bool in_canvas(const OutputCoord & oc)
    {
        return oc.x >= canvas_start_.x && oc.y >= canvas_start_.y &&
            oc.x <= canvas_.x && oc.x <= canvas_.y;
    }
};

Drawer::Drawer(const OutputCoord & canvas)
    : imp_(new Impl(canvas, OutputCoord(- canvas.x / 2., - canvas.y / 2.)))
{
    std::string background_color("white");
    imp_->shapes_.push_back(
        std::make_shared<svg_rect>(OutputCoord(imp_->canvas_start_.x, imp_->canvas_start_.y),
                                   OutputCoord(imp_->canvas_.x, imp_->canvas_.y), background_color));
}

Drawer::~Drawer()
{
    delete imp_;
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
    static const double e(exp(1));

    double s(2. * exp(-star.vmag_ / e));
    OutputCoord coord(imp_->projection_->project(star.pos_));
    if (! imp_->in_canvas(coord))
        return;

    auto c(std::make_shared<svg_circle>(coord, s, black, .5 * s, white));
    imp_->shapes_.push_back(c);

//    if (star.vmag_ < 4. && ! star.common_name_.empty())
//        imp_->board << Lb::Text(-star.pos_.ra + s, star.pos_.dec, star.common_name_, Lb::Fonts::Helvetica, 3.);
}

void Drawer::draw(const std::vector<ln_equ_posn> & path)
{
    std::vector<OutputCoord> ret;
    for (auto i(path.begin()), i_end(path.end());
         i != i_end; ++i)
        ret.push_back(imp_->projection_->project(*i));
    imp_->shapes_.push_back(std::make_shared<svg_cbezier>(std::move(ret), "#888888", 0.1));
}

void Drawer::draw(const std::string & body, const ln_equ_posn & pos)
{
    imp_->shapes_.push_back(std::make_shared<svg_text>(body, imp_->projection_->project(pos), 4.));
}
