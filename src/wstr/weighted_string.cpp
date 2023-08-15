#include "weighted_string.h"

namespace wstr
{

void weighted_string::set_gap(char gap)
{
    _gap = gap;
}

bool weighted_string::has_gap() const
{
    return _gap != NO_GAP;
}

char weighted_string::gap() const
{
    return _gap;
}

std::string weighted_string::heaviest() const
{
    return heaviest(true);
}

std::string weighted_string::heaviest_ungap() const
{
    return heaviest(false);
}

std::string weighted_string::heaviest(bool with_gap) const
{
    std::string h = "";

    for (const w_char& wc : *this) {
        char c = wc.heaviest_value();
        
        if (with_gap || c != _gap) {
            h += c;
        }
    }

    return h;
}

}