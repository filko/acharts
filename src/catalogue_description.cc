#include "catalogue_description.hh"

#include <sstream>

double parse_double(const std::string & in)
{
    std::stringstream s{in};
    double d;
    s >> d;
    return d;
}

double parse_sign(const std::string & in)
{
    if ("-" == in)
        return -1;

    return +1;
}

const Star parse_line_into_star(const CatalogParsingDescription & description, const std::string & line)
{
    ln_equ_posn pos{};
    std::string name;
    double vmag{};

    double dec_sign{1};

    for (auto const & desc : description.descriptions)
    {
        try
        {
            std::string part{line.substr(desc.start - 1, desc.len)};

#define CF CatalogParsingDescription::Field
            switch (desc.field)
            {
                case CF::Name:
                    name = part;
                    break;
                case CF::RAh:
                    pos.ra += 15.0 * parse_double(part);
                    break;
                case CF::RAm:
                    pos.ra += parse_double(part) / 4.;
                    break;
                case CF::RAs:
                    pos.ra += parse_double(part) / 240.;
                    break;
                case CF::DE_:
                    dec_sign = parse_sign(part);
                    break;
                case CF::DEd:
                    pos.dec += parse_double(part);
                    break;
                case CF::DEm:
                    pos.dec += parse_double(part) / 60.;
                    break;
                case CF::DEs:
                    pos.dec += parse_double(part) / 3600.;
                    break;
                case CF::Vmag:
                    vmag = parse_double(part);
                    break;
            }
#undef CF
        }
        catch (const std::out_of_range &)
        {
            throw std::runtime_error(""); // skipping the line
        }
    }
    pos.dec *= dec_sign;
    return Star(name, pos, vmag);
}

/*
    Bright Star Catalogue

    5- 14  A10    ---     Name     Name, generally Bayer and/or Flamsteed name
    76- 77  I2     h       RAh      ?Hours RA, equinox J2000, epoch 2000.0 (1)
    78- 79  I2     min     RAm      ?Minutes RA, equinox J2000, epoch 2000.0 (1)
    80- 83  F4.1   s       RAs      ?Seconds RA, equinox J2000, epoch 2000.0 (1)
        84  A1     ---     DE-      ?Sign Dec, equinox J2000, epoch 2000.0 (1)
    85- 86  I2     deg     DEd      ?Degrees Dec, equinox J2000, epoch 2000.0 (1)
    87- 88  I2     arcmin  DEm      ?Minutes Dec, equinox J2000, epoch 2000.0 (1)
    89- 90  I2     arcsec  DEs      ?Seconds Dec, equinox J2000, epoch 2000.0 (1)
   103-107  F5.2   mag     Vmag     ?Visual magnitude
*/
CatalogParsingDescription descriptions =
{ {
        { 5, 10, CatalogParsingDescription::Field::Name },
        { 76, 2, CatalogParsingDescription::Field::RAh },
        { 78, 2, CatalogParsingDescription::Field::RAm },
        { 80, 4, CatalogParsingDescription::Field::RAs },
        { 84, 1, CatalogParsingDescription::Field::DE_ },
        { 85, 2, CatalogParsingDescription::Field::DEd },
        { 87, 2, CatalogParsingDescription::Field::DEm },
        { 89, 2, CatalogParsingDescription::Field::DEs },
        { 103, 5, CatalogParsingDescription::Field::Vmag },
} };
