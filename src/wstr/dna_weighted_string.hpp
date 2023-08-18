#pragma once

#include "weighted_string.hpp"

namespace wstr
{

//! Standard DNA alphabet
inline constexpr char dna_alph[] = "ACGT";

//! DNA alphabet with gaps
inline constexpr char dna_alph_gap[] = "ACGT-";

// Gap of the alphabet
inline constexpr char dna_gap = '-';

//! Extended DNA alphabet, a letter can represent other letters from the DNA alphabet
static const std::unordered_map<char, std::string> dna_ext_alph = {
    {'R', "GA"},
    {'Y', "TC"},
    {'M', "AC"},
    {'K', "GT"},
    {'S', "GC"},
    {'W', "AT"},
    {'H', "ACT"},
    {'B', "GCT"},
    {'V', "GCA"},
    {'D', "GAT"},
    {'N', "GATC"}
};

//! Specific container for DNA
/*!
  * \tparam alph    Which alphabet we use (with or without gap)
  *
  * What is specific about the dna container is that when we ask the probability
  * of a character from the extended alphabet, the result is the sum of probabilities
  * of all letters which are represented by this letter.
 */
template <const char* alph>
class dna_container : public w_char_array<alph>
{
    public:

        double at(const char& key) const
        {
            if (dna_ext_alph.find(key) != dna_ext_alph.end()) {
                double p = 0.;
                for (const char c : dna_ext_alph.at(key)) {
                    p += w_char_array<alph>::at(c);
                }
                return p;
            }

            return w_char_array<alph>::at(key);
        }
};

//! Weighted string for the dna alphabet with gaps
class w_string_dna_gap : public weighted_string<dna_container<dna_alph_gap>>
{
    public:

        inline w_string_dna_gap() : weighted_string<dna_container<dna_alph_gap>>()
        {
            set_gap(dna_gap);
        }
};

//! This function is mandatory since the conversion cannot be done by itself
template <
    template <class, class> class Collection,
    class Allocator
>
std::istream& operator>>(std::istream& in, Collection<w_string_dna_gap, Allocator>& wsc)
{
    operator>>(in, reinterpret_cast<Collection<weighted_string<dna_container<dna_alph_gap>>, Allocator>&>(wsc));

    // Mandatory since the weighted strings are not constructed with the w_string_dna_gap constructor when casted
    for (w_string_dna_gap& ws : wsc) {
        ws.set_gap(dna_gap);
    }

    return in;
}

//! Weighted string for the dna alphabet without gaps
typedef weighted_string<dna_container<dna_alph>> w_string_dna;

//! Collection of dna weighted strings
typedef weighted_string_collection<w_string_dna> w_string_dna_collection;

//! Collection of dna weighted strings with gaps
typedef weighted_string_collection<w_string_dna_gap> w_string_dna_gap_collection;

}