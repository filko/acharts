#ifndef GRAPH_DRAWER_HH
#define GRAPH_DRAWER_HH 1

#include <memory>

#include "canvas.hh"
#include "stars.hh"
#include "projection.hh"
#include "solar_object.hh"

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

    explicit Drawer(const CanvasPoint & canvas);
    ~Drawer();

    void set_projection(const std::shared_ptr<Projection> & proj);

    void store(const char * filename) const;

    void draw(const Star & star);
    void draw(const SolarObject & object, double JD, object_rendering_type type = magnitudo, bool label = false);
    void draw(const std::vector<ln_equ_posn> & path);
    void draw(const std::string & body, const ln_equ_posn & pos);
};

#endif
