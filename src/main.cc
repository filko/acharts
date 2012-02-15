#include <libnova/libnova.h>
#include <iostream>

#include "config.hh"
#include "drawer.hh"
#include "projection.hh"
#include "stars.hh"

int main(int arc, char * arv[])
{
    try
    {
        Config config(arc, arv);

        auto loc(config.location());
        std::cout << loc.first << ' ' << loc.second << std::endl;

        return 0;
        Catalogue cat;
        cat.load("catalog");

        std::cerr << "Catalog size: " << cat.size() << std::endl;

        OutputCoord canvas = { 297., 210. };
        ln_equ_posn apparent_canvas = { 0., 25. },
            center = { 84., -1. };

        std::shared_ptr<Projection> projection(ProjectionFactory::create(canvas, apparent_canvas, center));

        Drawer drawer(canvas);
        drawer.set_projection(projection);

        std::size_t k(0);
        for (auto i(cat.get_stars().begin()), i_end(cat.get_stars().end());
             i != i_end ; ++i, ++k)
        {
//        std::cout  << i->pos_.ra << '\t' << i->pos_.dec << '\t' << i->vmag_ << '\t' << i->common_name_ << '\n';
//        if (i->vmag_ < 4.) // &&
//            (i->pos_.ra > 4.5 / 24 * 360) && (i->pos_.ra < 6.5 / 24 * 360) &&
//            (i->pos_.dec > -12.) && (i->pos_.dec < 12.))
            {
                drawer.draw(*i);
                std::cerr << '.';
            }
        }
        std::cerr << '\n';


        for (double x(0); x < 359.9; x += 30.)
        {
            std::vector<ln_equ_posn> path;
            for (double y(-88.); y < 88.1; y += .5)
            {
                path.push_back({x, y});
            }
            drawer.draw(path);
            drawer.draw(std::to_string(int(x)), {x, 0.});
        }

        for (double y(-60); y < 60.1; y += 30.)
        {
            std::vector<ln_equ_posn> path;
            for (double x(0.); x < 360.1; x += .5)
            {
                path.push_back({x, y});
            }
            drawer.draw(path);
            drawer.draw(std::to_string(int(y)), {0., y});
        }

        drawer.store("test-2.svg");
    }
    catch (const std::exception & e)
    {
        std::cerr << "Uncaught std::exception:\n\t" << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
