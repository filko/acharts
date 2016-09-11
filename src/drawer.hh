#ifndef GRAPH_DRAWER_HH
#define GRAPH_DRAWER_HH 1

#include <deque>
#include <memory>

#include "bezier.hh"
#include "canvas.hh"
#include "stars.hh"
#include "projection.hh"
#include "solar_object.hh"
#include "track.hh"

class Drawer
{
    struct Implementation;
    std::unique_ptr<Implementation> imp_;

public:
    enum object_rendering_type
    {
        magnitudo,
        sdiam
    };

    Drawer(const CanvasPoint & canvas, double canvas_margin, const std::string & style);
    ~Drawer();
    Drawer(const Drawer &) = delete;
    Drawer & operator=(const Drawer &) = delete;

    void set_projection(const std::shared_ptr<Projection> & proj);

    void store(const char * filename) const;

    void draw(const std::deque<Star> & stars, const std::string & group_id);
    void draw(const SolarObject & object, double JD, object_rendering_type type, bool label);
    void draw(const std::deque<std::shared_ptr<const SolarObject>> & object, double JD, const std::string & group_id, object_rendering_type type, bool label);
    void draw(const std::vector<ln_equ_posn> & path, double width = 0.1);
    void draw(const std::string & body, const ln_equ_posn & pos);
    void draw(const Track & track, const std::shared_ptr<const SolarObject> & object);
};

const std::deque<BezierCurve> create_bezier_from_track(const std::shared_ptr<Projection> & projection,
                                           const Track & track, const std::shared_ptr<const SolarObject> & object);

const std::deque<BezierCurve> create_bezier_from_path(const std::shared_ptr<Projection> & projection,
                                                      const std::vector<ln_equ_posn> & path);

#endif
