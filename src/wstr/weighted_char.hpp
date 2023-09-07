#pragma once

#include "weighted_element.hpp"

namespace wstr
{

//! A class for weighted character
/*!
  * \tparam Container   The container type for probabilities.
  *
  * The Container type should be the same than for a weighted element except that it should add the following method:
  * - char heaviest_non_gap(char gap) const         : Return the value with the heaviest probability which is not a gap character
  *
  * TODO:   This is a bandage to able weighted char to return the heaviest non gap character.
  *         We miss some methods in weighted_element which could handle that but I don't have time to think to such generic methods.
  *
  * \sa wstr::wc_map
  * \sa wstr::wc_array
 */
template <
    class Container
>
class weighted_char : public weighted_element<char, Container>
{
    public:

        using weighted_element<char, Container>::weighted_element;

        //! Return the value which has the heighest probability and which is not a gap
        char heaviest_non_gap_value(char gap) const
        {
            return this->probabilities().heaviest_non_gap(gap);
        }

        //! Return the probability of the heaviest value which is not a gap
        double heaviest_non_gap_proba(char gap) const
        {
            return this->probabilities().at(heaviest_non_gap_value(gap));
        }
};


//! Container for weighted character based on unordered map
/*!
  * \sa w_map
 */ 
template<
    class Hash = std::hash<char>,
    class KeyEqual = std::equal_to<char>,
    class Allocator = std::allocator<std::pair<const char, double>>
>
class wc_map : public w_map<char, Hash, KeyEqual, Allocator> 
{
    public:

        using w_map<char, Hash, KeyEqual, Allocator>::w_map;

        char heaviest_non_gap(char gap) const
        {
            if (0 == this->size()) {
                throw std::runtime_error("There is no value with probability greater than 0.");
            }

            return std::max_element(this->begin(), this->end(), [&gap](auto& p1, auto& p2) {
                if (p1.first == gap) { return true; }
                if (p2.first == gap) { return false; }
                return p1.second < p2.second;
            })->first;
        }
};


//! Container for weighted character based on array
/*!
  * \sa w_array
 */ 
template<
    std::size_t N,
    class Translator
>
class wc_array : public w_array<char, N, Translator>
{
    public:

        using w_array<char, N, Translator>::w_array;

        char heaviest_non_gap(char gap) const
        {
            size_t pos = Translator::get_indice(gap);
            double max = -1.;
            size_t maxp = -1;

            for (size_t i = 0; i < N; ++i) {
                if (std::array<double, N>::at(i) > max && i != pos) {
                    max = std::array<double, N>::at(i);
                    maxp = i;
                }
            }

            if (maxp == -1) {
                throw std::runtime_error("Your alphabet only have one letter, so the heaviest letter without this letter doesn't exist\n");
            }

            return Translator::get_element(maxp);
        }
};

}