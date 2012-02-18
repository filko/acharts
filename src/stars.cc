#include "stars.hh"

Star::Star(const std::string & cname,
           lnh_equ_posn & hpos,
           double vm)
    : common_name_(cname),
      vmag_(vm)
{
    ln_hequ_to_equ(&hpos, &pos_);
}
