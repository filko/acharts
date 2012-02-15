add_executable(charts
  config.cc
  config_parser_i_hate_boost.cc
  drawer.cc
  main.cc
  now.cc
  projection.cc
  stars.cc
)

target_link_libraries(charts nova rt)
