#include <libnova/libnova.h>
#include <deque>
#include <fstream>
#include <iostream>
#include <sstream>

#include "config.hh"
#include "drawer.hh"
#include "exceptions.hh"
#include "projection.hh"
#include "scene_storage.hh"
#include "stars.hh"
#include "svg_painter.hh"
#include "types.hh"

/*
struct ScenePrinter
    : boost::static_visitor<>
{
    void operator()(const scene::Object & o) 
    {
//        std::cout << '{' << o.pos.x << ',' << o.pos.y << '}';
    }

    void operator()(const scene::Group & g) 
    {
        std::cout << "g(" << g.id << "){";
        for_each(g.elements.begin(), g.elements.end(),
                 boost::apply_visitor(*this));
        std::cout << "}";
    }

    void operator()(const scene::Rectangle &) 
    {
    }
    void operator()(const scene::Line &)
    {}
    void operator()(const scene::Path & p)
    {
        std::cout << p.path.size() << ',';
    }
    void operator()(const scene::Text &)
    {}
};
*/

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

        Drawer drawer(canvas, config.canvas_margin(), style);
        drawer.set_projection(projection);

        scene::Storage scn(projection);

        {
            scene::Group gr{"rectangle", "background", {}};
            gr.elements.push_back(scene::Rectangle{CanvasPoint(-canvas.x / 2., -canvas.y / 2.),
                        CanvasPoint(canvas.x, canvas.y)});
            scn.add_group(std::move(gr));
        }

        std::cout << "Loading catalogues... " << std::flush;
        for (auto c(config.begin_catalogues()), c_end(config.end_catalogues());
             c != c_end; ++c)
        {
            std::cout << c->path() << ", " << std::flush;
            c->load();

            std::deque<Star> stars;
            std::copy(c->begin_stars(), c->end_stars(), std::back_inserter(stars));
            drawer.draw(stars, c->path());

            {
                std::deque<scene::Element> objs;
                for (auto const & star : stars)
                {
                    objs.push_back(scene::Object{projection->project(star.pos_), star.vmag_});
                }
                scn.add_group(scene::Group{"catalog", c->path(), std::move(objs)});
            }
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

            drawer.draw(planets, config.t(), "planets", Drawer::magnitudo, config.planets_labels());

            {
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
        }

        if (config.moon())
        {
            auto const & moon{*solar_manager.get("moon")};
            drawer.draw(moon, config.t(), Drawer::sdiam, true);

            std::deque<scene::Element> obj;
            auto pos{moon.get_equ_coords(t)};
            obj.push_back(scene::ProportionalObject{projection->project(pos),
                        moon.get_sdiam(t) * projection->scale_at_point(pos) / 3600.,
                        moon.name()});
            scn.add_group(scene::Group{"solar_system", "moon",
                        std::move(obj)
                });
        }

        if (config.sun())
        {
            auto const & sun{*solar_manager.get("sun")};
            drawer.draw(sun, config.t(), Drawer::sdiam, true);

            std::deque<scene::Element> obj;
            auto pos{sun.get_equ_coords(t)};
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
            for (auto track(config.begin_tracks()), track_end(config.end_tracks());
                 track != track_end; ++track)
            {
                std::cout << track->name << " " << std::flush;
                drawer.draw(*track, solar_manager.get(track->name));
                auto path(create_path_from_track(projection, *track, solar_manager.get(track->name)));
                auto beziers(create_bezier_from_path(path));
                for (auto const & b : beziers)
                {
                    tracks.elements.push_back(scene::Path{b});
                }

                auto blind_beziered(interpolate_bezier(path));
                for (int i(0), i_end(blind_beziered.size());
                     i < i_end; i += track->interval_ticks)
                {
                    auto p(blind_beziered[i]);
                    tracks.elements.push_back(scene::DirectedObject{p.p, p.perpendicular});
                }
            }
            scn.add_group(std::move(tracks));
            std::cout << "done." << std::endl;
        }

        {
            scene::Group meridians{"grid", "meridians", {}};
            for (double x(0); x < 360.1; x += 15.)
            {
                std::vector<ln_equ_posn> path;
                for (double y(-88.); y < 88.1; y += 2.)
                {
                    path.push_back({x, y});
                }
                drawer.draw(path);
                drawer.draw(stringify(angle{x}, as_hour), {x, 0.});

                auto bezier{create_bezier_from_path(projection, path)};
                for (auto const b : bezier)
                {
                    meridians.elements.push_back(scene::Path{b});
                }
                meridians.elements.push_back(scene::Text{stringify(angle{x}, as_hour), projection->project({x, 0})});
            }
            scn.add_group(std::move(meridians));
        }

        {
            scene::Group parallels{"grid", "parallels", {}};
            for (double y(-60); y < 60.1; y += 10.)
            {
                std::vector<ln_equ_posn> path;
                for (double x(0.); x < 360.1; x += 2.)
                {
                    path.push_back({x, y});
                }
                drawer.draw(path);
                drawer.draw(stringify(angle{y}, as_degree), {0., y});

                auto bezier{create_bezier_from_path(projection, path)};
                for (auto const b : bezier)
                {
                    parallels.elements.push_back(scene::Path{b});
                }
                parallels.elements.push_back(scene::Text{stringify(angle{y}, as_degree), projection->project({0., y})});
            }
            scn.add_group(std::move(parallels));
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

            scene::Group ecliptic{"grid", "ecliptic", {}};
            auto bezier{create_bezier_from_path(projection, path)};
            for (auto const b : bezier)
            {
                ecliptic.elements.push_back(scene::Path{b});
            }
            scn.add_group(std::move(ecliptic));
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

            scene::Group horizon{"grid", "horizon", {}};
            auto bezier{create_bezier_from_path(projection, path)};
            for (auto const b : bezier)
            {
                horizon.elements.push_back(scene::Path{b});
            }
            scn.add_group(std::move(horizon));
        }

        drawer.store(config.output().c_str());

        std::ofstream of("test-2.svg");
        if (! of)
            throw std::runtime_error("Can't open file '" + std::string("test-2.svg") + "' for writing " + std::strerror(errno));

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
