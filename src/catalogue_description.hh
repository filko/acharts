#ifndef ACHARTS_CATALOGUE_DESCRIPTION_HH
#define ACHARTS_CATALOGUE_DESCRIPTION_HH

#include <vector>

#include "stars.hh"

struct CatalogParsingDescription
{
    enum class Field
    {
        Name,
        RAh, RAm, RAs,
        DE_, DEd, DEm, DEs,
        Vmag
    };

    struct Entity
    {
        std::size_t start, len;
        Field field;
    };
    std::vector<Entity> descriptions;
};

const Star parse_line_into_star(const CatalogParsingDescription description, const std::string & line);

extern CatalogParsingDescription descriptions;

#endif
