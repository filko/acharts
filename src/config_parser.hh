#ifndef CHART_CONFIG_PARSER_HH

#include <istream>
#include <functional>

namespace config_parser
{

typedef std::function<void (const std::string &, const std::string &)> Callback;

void parse_config(std::istream & os, const Callback & callback);
double parse_angle(const std::string & in);
double parse_timestamp(const std::string & in);

}

#endif
