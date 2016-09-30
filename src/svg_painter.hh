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
#ifndef ACHARTS_SVG_PAINTER_HH_
#define ACHARTS_SVG_PAINTER_HH_ 1

#include <boost/variant/static_visitor.hpp>
#include <memory>

#include "scene.hh"

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
    void operator()(const scene::ProportionalObject & o);
    void operator()(const scene::LabelledObject & lo);
    void operator()(const scene::DirectedObject & d);
    void operator()(const scene::Group & g);
    void operator()(const scene::Rectangle & r);
    void operator()(const scene::Line & l);
    void operator()(const scene::Path & p);
    void operator()(const scene::Text & t);
};

#endif
