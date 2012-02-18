#ifndef CHART_EXCEPTIONS_HH
#define CHART_EXCEPTIONS_HH

#include <stdexcept>

class ConfigError
    : public std::runtime_error
{
public:
    explicit ConfigError(const std::string & wha)
        : std::runtime_error("Configuration error:\n" + wha)
    {
    }
};

class ConfigValueError
    : public ConfigError
{
public:
    explicit ConfigValueError(const std::string & name, const std::string & value)
        : ConfigError("Cannot parse value.  Expected <" + name + ">, got '" + value + "'.")
    {
    }
};

class InternalError
    : public std::logic_error
{
public:
    explicit InternalError(const std::string & wha)
        : std::logic_error(wha)
    {
    }
};

#endif
