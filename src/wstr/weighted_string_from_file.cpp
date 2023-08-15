#include "weighted_string.h"

#include <iostream>

namespace wstr
{

inline bool& ws_strict()
{
    static bool strict = true;
    return strict;
}

inline bool& ws_gap()
{
    static bool has_gap = false;
    return has_gap;
}

inline double& ws_precision()
{
    static double precision = 0.;
    return precision;
}

void construct_ws_from_file(std::istream& input, weighted_string& ws, size_t n, const std::string& alph)
{
    if (ws_gap()) {
        ws.set_gap(alph.back());
    }

    double v;

    for (int i = 0; i < n; ++i) {
        w_char::container_t wc;

        for (char c : alph) {
            input >> v;
            
            // Avoid adding useless probabilities
            if (0. != v) {
                wc[c] = v;
            }
        }
        
        ws.emplace_back(w_char(wc, ws_strict(), ws_precision()));
    }
}

std::istream& operator>>(std::istream& input, weighted_string& ws)
{
    size_t n;
    std::string alph;

    input >> n;
    input >> alph;

    construct_ws_from_file(input, ws, n, alph);

    return input;
}

std::istream& operator>>(std::istream& input, ws_collection& wsv)
{
    size_t l, n;
    std::string alph;

    input >> l;
    input >> alph;

    wsv.clear();

    for (size_t _ = 0; _ < l; ++_) {
        input >> n;
        weighted_string ws;

        construct_ws_from_file(input, ws, n, alph);

        wsv.push_back(ws);
    }

    return input;
}

std::istream& operator>>(std::istream& input, const ws_manip& manip)
{
    return input;
}

std::istream& ws_strict(std::istream& input)
{
    ws_strict() = true;
    return input;
}

std::istream& ws_not_strict(std::istream& input)
{
    ws_strict() = false;
    return input;
}

std::istream& ws_gap(std::istream& input)
{
    ws_gap() = true;
    return input;
}

std::istream& ws_no_gap(std::istream& input)
{
    ws_gap() = false;
    return input;
}

void _set_prec(double p)
{
    ws_precision() = p;
}

ws_manip ws_set_precision(double p)
{ 
    ws_precision() = p;
    return ws_manip();
}

}