#include <libnova/libnova.h>
#include <iostream>
#include <deque>

#include "config.hh"
#include "drawer.hh"
#include "exceptions.hh"
#include "projection.hh"
#include "stars.hh"
#include "types.hh"

int main(int arc, char * arv[])
{
    try
    {
        std::cout << "Processing configs... " << std::flush;
        Config config(arc, arv);
        std::cout << "done." << std::endl;

        //        return 0;

        ln_lnlat_posn observer(config.location());
        const double t(config.t());

        CanvasPoint canvas(config.canvas_dimensions());
        ln_equ_posn apparent_canvas(config.projection_dimensions()),
            center(config.projection_centre());

        std::shared_ptr<Projection> projection(ProjectionFactory::create(config.projection_type(),
                                                                         canvas, apparent_canvas, center));

        if (config.projection_level() == "horizon")
        {
            ln_hrz_posn hor;
            ln_get_hrz_from_equ(&center, &observer, t, &hor);
            ln_hrz_posn hor2(hor);
            hor2.az += 1.0;
            ln_equ_posn equ;
            ln_get_equ_from_hrz(&hor2, &observer, t, &equ);
            CanvasPoint cp(equ.ra - center.ra, equ.dec - center.dec);
            if (cp.x > 180.)
                cp.x -= 360.;

            std::cout << cp.x << ", " << cp.y << std::endl;
            projection->rotate_to_level(cp);
        }

        Drawer drawer(canvas, config.canvas_margin());
        drawer.set_projection(projection);

        std::cout << "Loading catalogues... " << std::flush;
        for (auto c(config.begin_catalogues()), c_end(config.end_catalogues());
             c != c_end; ++c)
        {
            std::cout << c->path() << ", " << std::flush;
            c->load();

            std::deque<Star> stars;
            std::copy(c->begin_stars(), c->end_stars(), std::back_inserter(stars));
            drawer.draw(stars);
        }
        std::cout << "done." << std::endl;

        std::cout << "Loading solar objects..." << std::flush;
        SolarObjectManager solar_manager;
        std::vector<std::shared_ptr<const SolarObject>> planets;
        auto planet_names(config.planets());
        for (auto const & p : planet_names)
        {
            planets.push_back(solar_manager.get(p));
        }

        for (auto const & p : planets)
        {
            drawer.draw(*p, config.t(), Drawer::magnitudo, config.planets_labels());
        }

        if (config.moon())
        {
            drawer.draw(*solar_manager.get("moon"), config.t(), Drawer::sdiam, true);
        }

        if (config.sun())
        {
            drawer.draw(*solar_manager.get("sun"), config.t(), Drawer::sdiam, true);
        }
        std::cout << "done." << std::endl;

        std::cout << "Drawing tracks... " << std::flush;
        for (auto track(config.begin_tracks()), track_end(config.end_tracks());
             track != track_end; ++track)
        {
            std::cout << track->name << " " << std::flush;
            drawer.draw(*track, solar_manager.get(track->name));
        }
        std::cout << "done." << std::endl;

        for (double x(0); x < 360.1; x += 15.)
        {
            std::vector<ln_equ_posn> path;
            for (double y(-88.); y < 88.1; y += 2.)
            {
                path.push_back({x, y});
            }
            drawer.draw(path);
            drawer.draw(stringify(angle{x}, as_hour), {x, 0.});
        }

        for (double y(-60); y < 60.1; y += 10.)
        {
            std::vector<ln_equ_posn> path;
            for (double x(0.); x < 360.1; x += 2.)
            {
                path.push_back({x, y});
            }
            drawer.draw(path);
            drawer.draw(stringify(angle{y}, as_degree), {0., y});
        }

        // ecliptic
        {
            std::vector<ln_equ_posn> path;
            double t(config.t());
            for (double x(0); x < 360.1; x += 2.)
            {
                ln_lnlat_posn in{x, 0.};
                ln_equ_posn out;
                ln_get_equ_from_ecl(&in, t, &out);
                path.push_back(out);
            }
            drawer.draw(path, 0.2);
        }

        // horizon
        {
            std::vector<ln_equ_posn> path;
            for (double x(0); x < 360.1; x += 2.)
            {
                ln_hrz_posn in{x, 0.};
                ln_equ_posn out;
                ln_get_equ_from_hrz(&in, &observer, t, &out);
                path.push_back(out);
            }
            drawer.draw(path, 0.3);
        }

        drawer.store("test-2.svg");
    }
    catch (const ConfigError & e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::exception & e)
    {
        std::cerr << "Uncaught std::exception:\n\t" << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
