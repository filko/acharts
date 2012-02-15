#ifndef CHARTS_CONFIG_HH
#define CHARTS_CONFIG_HH 1

#include <utility>

class Config
{
    struct Implementation;
    Implementation * imp_;
public:
    Config(int arc, char * arv[]);
    ~Config();

    std::pair<double, double> location() const;
};

#endif
