#ifndef CHART_CONFIG_PARSER_HH

#include <istream>
#include <functional>

namespace config_parser
{

typedef std::function<void (const std::string &, const std::string &)> ValueCallback;
typedef std::function<void (const std::string &)> SectionCallback;

void parse_config(std::istream & os, const SectionCallback & scal, const ValueCallback & vcal);
double parse_angle(const std::string & in);
double parse_timestamp(const std::string & in);
double parse_length(const std::string & in);
bool parse_boolean(const std::string & in);
int parse_integer(const std::string & in);

}

#endif
