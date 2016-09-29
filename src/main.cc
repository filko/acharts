#include <cstring>
#include <libnova/libnova.h>
#include <deque>
#include <fstream>
#include <iostream>
#include <sstream>

#include "config.hh"
#include "drawer.hh"
#include "exceptions.hh"
#include "projection.hh"
#include "scene.hh"
#include "solar_object.hh"
#include "stars.hh"
#include "svg_painter.hh"
#include "types.hh"

int main(int arc, char * arv[])
{
    try
    {
        std::cout << "Processing configs... " << std::flush;
        Config config(arc, arv);
        std::cout << "done." << std::endl;

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

            projection->rotate_to_level(cp);
        }

        std::string style;
        if (! config.stylesheet().empty())
        {
            std::ifstream f(config.stylesheet());
            if (! f)
                throw ConfigError("Stylesheet '" + config.stylesheet() + "' couldn't be opened.");

            std::stringstream buf;
            buf << f.rdbuf();
            style = buf.str();
        }

        scene::Scene scn(projection);

        {
            scene::Group gr{"rectangle", "background", {}};
            gr.elements.push_back(scene::Rectangle{CanvasPoint(-canvas.x / 2., -canvas.y / 2.),
                        CanvasPoint(canvas.x, canvas.y)});
            scn.add_group(std::move(gr));
        }

        std::cout << "Loading catalogues... " << std::flush;
        for (auto & c : config.view<Catalogue>())
        {
            std::cout << c.path() << std::flush;
            std::size_t count{c.load()};
            std::cout << "(" << count << "), " << std::flush;

            std::deque<scene::Element> objs;
            for (auto s(c.begin_stars()), s_end(c.end_stars()) ; s != s_end ; ++s)
            {
                objs.push_back(scene::Object{projection->project(s->pos_), s->vmag_});
            }
            scn.add_group(scene::Group{"catalog", c.path(), std::move(objs)});
        }
        std::cout << "done." << std::endl;

        std::cout << "Loading solar objects..." << std::flush;
        SolarObjectManager solar_manager;

        {
            std::deque<std::shared_ptr<const SolarObject>> planets;
            auto planet_names(config.planets());
            for (auto const & p : planet_names)
            {
                planets.push_back(solar_manager.get(p));
            }

            std::deque<scene::Element> objs;
            for (auto const & planet : planets)
            {
                objs.push_back(
                    scene::LabelledObject{
                        projection->project(planet->get_equ_coords(t)), planet->get_magnitude(t),
                            planet->name()});
            }
            scn.add_group(scene::Group{"solar_system", "planets", std::move(objs)});
        }

        if (config.moon())
        {
            auto const & moon(*solar_manager.get("moon"));
            std::deque<scene::Element> obj;
            auto pos(moon.get_equ_coords(t));
            obj.push_back(scene::ProportionalObject{projection->project(pos),
                        moon.get_sdiam(t) * projection->scale_at_point(pos) / 3600.,
                        moon.name()});
            scn.add_group(scene::Group{"solar_system", "moon",
                        std::move(obj)
                        });
        }

        if (config.sun())
        {
            auto const & sun(*solar_manager.get("sun"));
            std::deque<scene::Element> obj;
            auto pos(sun.get_equ_coords(t));
            obj.push_back(scene::ProportionalObject{projection->project(pos),
                        sun.get_sdiam(t) * projection->scale_at_point(pos) / 3600.,
                        sun.name()});
            scn.add_group(scene::Group{"solar_system", "sun",
                        std::move(obj)
                        });
        }
        std::cout << "done." << std::endl;

        {
            std::cout << "Drawing tracks... " << std::flush;
            scene::Group tracks{"tracks", "track_container", {}};
            for (auto const & track : config.view<Track>())
            {
                std::cout << track.name << " " << std::flush;
                tracks.elements.push_back(scene::build_track(track, projection, solar_manager));
            }
            scn.add_group(std::move(tracks));
            std::cout << "done." << std::endl;
        }

        std::cout << "Drawing grids... " << std::flush;
        for (auto grid : config.view<Grid>())
        {
            scn.add_group(scene::build_grid(grid, projection, observer, t));
        }
        std::cout << "done." << std::endl;

        std::cout << "Drawing ticks... " << std::flush;
        for (auto tick : config.view<Tick>())
        {
            scn.add_group(scene::build_tick(tick, projection));
        }
        std::cout << "done." << std::endl;

        std::ofstream of(config.output().c_str());
        if (! of)
            throw std::runtime_error("Can't open file '" + config.output() + "' for writing " + std::strerror(errno));

        SvgPainter painter(of, canvas, config.canvas_margin(), style);
        boost::apply_visitor(painter, scn);
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
