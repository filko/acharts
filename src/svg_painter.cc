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

void SvgPainter::operator()(const scene::ProportionalObject & o)
{
    if (!imp_->in_canvas(o.pos))
        return;

    double stroke_width{0.2 * o.radius};
    os_ << "<circle cx='" << o.pos.x << "' cy='" << o.pos.y << "' "
        "r='" << o.radius << "' stroke-width='" << stroke_width << "' />\n"
        "<text x='" << o.pos.x + o.radius << "' y='" << o.pos.y << "' dy='0.5ex'>"
        << o.label << "</text>\n";
}

void SvgPainter::operator()(const scene::LabelledObject & lo)
{
    if (!imp_->in_canvas(lo.pos))
        return;

    double radius{mag2size(lo.mag)},
        stroke_width{0.2 * radius};
    os_ << "<circle cx='" << lo.pos.x << "' cy='" << lo.pos.y << "' "
        "r='" << radius << "' stroke-width='" << stroke_width << "' />\n"
        "<text x='" << lo.pos.x + radius << "' y='" << lo.pos.y << "' dy='0.5ex'>"
        << lo.label << "</text>\n";
}

void SvgPainter::operator()(const scene::DirectedObject & o)
{
    if (!imp_->in_canvas(o.pos))
        return;

    CanvasPoint s(o.pos - o.dir), e(o.pos + o.dir);
    os_ << "<line x1='" << s.x << "' y1='" << s.y << "' "
        "x2='" << e.x << "' y2='" << e.y << "'/>\n";
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

void SvgPainter::operator()(const scene::Path & path)
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

    std::deque<BezierCurve> visibles(1);
    {
        for (auto first(path.path.cbegin()), second(first + 1), end(path.path.cend());
             second != end; first = second, ++second)
        {
            if (imp_->CohenSutherland(first->p, second->p))
            {
                if (visibles.back().empty())
                    visibles.back().push_back(*first);
                visibles.back().push_back(*second);
            }
            else if (! visibles.back().empty())
                visibles.push_back(BezierCurve{});
        }
    }
    for (auto const & p : visibles)
    {
        if (p.empty())
            continue;
        os_ << "<path d='";
        auto prev(p.cbegin());
        os_ << 'M' << prev->p.x << ',' << prev->p.y << ' ';
        for (auto next(prev + 1), end(p.cend());
             next != end; prev = next, ++next)
        {
            os_ << 'C' << prev->cp.x << ',' << prev->cp.y << ' '
                << next->cm.x << ',' << next->cm.y << ' '
                << next->p.x << ',' << next->p.y << ' ';
        }
        os_ << "' />\n";
    }
}

void SvgPainter::operator()(const scene::Text & t)
{
    if (! imp_->in_canvas(t.pos))
        return;

    os_ << "<text x='" << t.pos.x << "' y='" << t.pos.y << "'>" << t.body << "</text>\n";
}
