add_executable(charts
  catalogue.cc
  config.cc
  config_parser_i_hate_boost.cc
  drawer.cc
  main.cc
  moon_and_sun.cc
  now.cc
  planet.cc
  projection.cc
  stars.cc
)

target_link_libraries(charts nova rt)
