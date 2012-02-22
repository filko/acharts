#ifndef GRAPH_DRAWER_HH
#define GRAPH_DRAWER_HH 1

#include <memory>

#include "stars.hh"
#include "projection.hh"

class Drawer
{
    struct Implementation;
    std::unique_ptr<Implementation> imp_;

public:
    explicit Drawer(const OutputCoord & canvas);
    ~Drawer();

    void set_projection(const std::shared_ptr<Projection> & proj);

    void store(const char * filename) const;

    void draw(const Star & star);
    void draw(const std::vector<ln_equ_posn> & path);
    void draw(const std::string & body, const ln_equ_posn & pos);
};

#endif
