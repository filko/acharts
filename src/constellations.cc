#include "constellations.hh"

const std::vector<std::pair<ln_equ_posn, ln_equ_posn>> constellation_edges = {
    #include "constellations.cc.in"
};
