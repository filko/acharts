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

struct svg_line
    : public svg_shape
{
    CanvasPoint start, end;
    double width;

    svg_line(const CanvasPoint & start, const CanvasPoint & end, double width)
        : start(start), end(end), width(width)
    {
    }

    virtual void flush(std::ostream & out)
    {
        out << "<line x1='" << start.x << "' y1='" << start.y << "' "
            "x2='" << end.x << "' y2='" << end.y << "' stroke='gray' stroke-width='" << width << "' />";
    }
};

struct svg_cbezier
    : public svg_shape
{
    BezierCurve path;
    std::string stroke;
    double width;

    svg_cbezier(const BezierCurve & path, const std::string & stroke, double width)
        : path(std::move(path)), stroke(stroke), width(width)
    {
    }

    virtual void flush(std::ostream & out)
    {
#if 0
        // drawing ugly control points, usefull for debugging
        for (auto const i : path)
        {
            out << "<circle cx='" << i.p.x << "' cy='" << i.p.y << "' r='2px' fill='black' />\n";
            out << "<circle cx='" << i.cm.x << "' cy='" << i.cm.y << "' r='1px' fill='blue' />\n";
            out << "<circle cx='" << i.cp.x << "' cy='" << i.cp.y << "' r='1px' fill='green' />\n";
        }
#endif

        out << "<path stroke='gray' stroke-width='" << width << "' fill='none'\n  d='";
        auto prev(path.cbegin());
        out << 'M' << prev->p.x << ',' << prev->p.y << ' ';
        for (auto next(prev + 1), i_end(path.cend());
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

struct svg_group
    : public svg_shape
{
    std::deque<std::shared_ptr<svg_shape>> children;
    const std::string id;
    const std::string _class;

    svg_group(const std::string & group_id, const std::string & _class) : id(group_id), _class(_class) { }

    void push_back(const std::shared_ptr<svg_shape> & shape)
    {
        children.push_back(shape);
    }

    virtual void flush(std::ostream & out)
    {
        out << "<g id=\"" << id << "\" class=\"" << _class << "\">\n";
        for (auto const & child : children)
        {
            child->flush(out);
        }
        out << "</g>\n";
    }
};

}

struct Drawer::Implementation
{
    std::shared_ptr<Projection> projection_;
    double canvas_margin_;
    const CanvasPoint canvas_, canvas_start_, canvas_end_;
    const std::string style_;
    std::deque<std::shared_ptr<svg_shape>> shapes_;

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

    double mag2size(double mag)
    {
        static const double e(exp(1));
        return 2.0 * exp(-mag / e) + 0.1;
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

    enum {
        inside = 0, // 0000
        left   = 1, // 0001
        right  = 2, // 0010
        bottom = 4, // 0100
        top    = 8  // 1000
    };

    int Outcode(const CanvasPoint & p)
    {
        int code(0);

        if (p.x < canvas_start_.x)
            code |= left;
        else if (p.x > canvas_end_.x)
            code |= right;

        if (p.y < canvas_start_.y)
            code |= bottom;
        else if (p.y > canvas_end_.y)
            code |= top;

        return code;
    }

    bool CohenSutherland(CanvasPoint p0, CanvasPoint p1)
    {
        int code0 = Outcode(p0);
        int code1 = Outcode(p1);
        bool ret(false);

        while (true)
        {
            if (!(code0 | code1))
            {
                ret = true;
                break;
            }
            else if (code0 & code1)
            {
                break;
            }
            else
            {
                CanvasPoint p;
                int code = code0 ? code0 : code1;

                // use formulas y = y0 + slope * (x - x0), x = x0 + (1 / slope) * (y - y0)
                if (code & top)
                {
                    p.x = p0.x + (p1.x - p0.x) * (canvas_end_.y - p0.y) / (p1.y - p0.y);
                    p.y = canvas_end_.y;
                }
                else if (code & bottom)
                {
                    p.x = p0.x + (p1.x - p0.x) * (canvas_start_.y - p0.y) / (p1.y - p0.y);
                    p.y = canvas_start_.y;
                }
                else if (code & right)
                {
                    p.y = p0.y + (p1.y - p0.y) * (canvas_end_.x - p0.x) / (p1.x - p0.x);
                    p.x = canvas_end_.x;
                }
                else if (code & left)
                {
                    p.y = p0.y + (p1.y - p0.y) * (canvas_start_.x - p0.x) / (p1.x - p0.x);
                    p.x = canvas_start_.x;
                }

                if (code == code0)
                {
                    p0 = p;
                    code0 = Outcode(p0);
                }
                else
                {
                    p1 = p;
                    code1 = Outcode(p1);
                }
            }
        }

        return ret;
    }

};

Drawer::Drawer(const CanvasPoint & canvas, double canvas_margin, const std::string & style)
    : imp_(new Implementation(canvas, canvas_margin, style))
{
    std::string background_color("white");
    imp_->shapes_.push_back(
        std::make_shared<svg_rect>(CanvasPoint(-imp_->canvas_.x / 2., -imp_->canvas_.y / 2.),
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
        "viewBox='" << -imp_->canvas_.x / 2. << ' ' << -imp_->canvas_.y / 2. <<
        ' ' << imp_->canvas_.x << ' ' << imp_->canvas_.y << "' "
        "xmlns='http://www.w3.org/2000/svg' version='1.1'>\n";

    of << "<style type=\"text/css\">\n" << imp_->style_ << "</style>\n";

    for (auto const shape : imp_->shapes_)
        shape->flush(of);

    of << "</svg>\n";
}

void Drawer::draw(const std::deque<Star> & stars, const std::string & group_id)
{
    static const std::string black("black");
    static const std::string white("white");

    std::shared_ptr<svg_group> group(std::make_shared<svg_group>(group_id, "stars"));
    for (auto const & star : stars)
    {
        CanvasPoint coord(imp_->projection_->project(star.pos_));
        if (! imp_->in_canvas(coord))
            continue;

        double s(imp_->mag2size(star.vmag_));
        group->push_back(std::make_shared<svg_circle>(coord, s, black, .2 * s, white));
    }
    imp_->shapes_.push_back(group);
}

void Drawer::draw(const std::vector<ln_equ_posn> & path, double width)
{
    std::vector<std::vector<CanvasPoint>> ret(1);
    for (auto const & i : path)
    {
        auto p(imp_->projection_->project(i));
        if (! p.nan())
            ret.back().push_back(p);
        else if (! ret.back().empty())
            ret.push_back(std::vector<CanvasPoint>());
    }

    std::vector<BezierCurve> strips(1);
    for (auto const & b : ret)
    {
        if (! strips.back().empty())
            strips.push_back(BezierCurve());

        auto beziered(interpolate_bezier(b));
        auto first(beziered.cbegin()), second(first + 1);
        for (auto end(beziered.cend());
             second != end; first = second, ++second)
        {
            if (imp_->CohenSutherland(first->p, second->p))
            {
                if (strips.back().empty())
                    strips.back().push_back(*first);
                strips.back().push_back(*second);
            }
            else if (! strips.back().empty())
                strips.push_back(BezierCurve());
        }
    }

    for (auto const & i : strips)
        if (! i.empty())
            imp_->shapes_.push_back(std::make_shared<svg_cbezier>(i, "#888888", width));
}

void Drawer::draw(const Track & track, const std::shared_ptr<const SolarObject> & object)
{
    std::vector<ln_equ_posn> path;
    std::vector<CanvasPoint> cpath;
    double step(track.mark_interval / track.interval_ticks);

    for (double t(track.start.val()); t <= track.end.val(); t += step)
    {
        auto coord(object->get_equ_coords(t));
        path.push_back(coord);
        cpath.push_back(imp_->projection_->project(coord));
    }
    draw(path);

    auto beziered(interpolate_bezier(cpath));
    for (int i(0), i_end(beziered.size());
         i < i_end; i += track.interval_ticks)
    {
        auto p(beziered[i]);
        if (! p.p.nan() && imp_->in_canvas(p.p))
            imp_->shapes_.push_back(
                std::make_shared<svg_line>((p.p - p.perpendicular), (p.p + p.perpendicular), 0.1));
    }
}

void Drawer::draw(const std::string & body, const ln_equ_posn & pos)
{
    auto coord(imp_->projection_->project(pos));
    if (coord.nan() || ! imp_->in_canvas(coord))
        return;

    imp_->shapes_.push_back(std::make_shared<svg_text>(body, coord, 4.));
}

void Drawer::draw(const SolarObject & object, double jd, object_rendering_type type, bool label)
{
    auto pos(object.get_equ_coords(jd));
    auto coord(imp_->projection_->project(pos));
    if (coord.nan() || ! imp_->in_canvas(coord))
        return;

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
