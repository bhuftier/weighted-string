#pragma once

#include <vector>
#include <cstring>
#include <iostream>

#include "weighted_char.hpp"

#define NO_GAP -1

namespace wstr
{

//! A class for weighted strings based on a collection (std::vector) of weighted characters.
/*! 
  * \tparam Container   Same as Container param for weighted_char
  * \tparam Allocator   The allocator for the weighted elements
  * 
  * This class is a public inheritance of std::vector for weighted character.
  * It means that you can use all vector methods like size(), clear()...
  * You can also have foreach like loop over weighted characters of the weighted string.
  *
  * A weighted string can have a gap character assigned to it.
  *
  * \sa wstr::weighted_char
 */
template<
    class Container,
    class Allocator = std::allocator<weighted_char<Container>>
>
class weighted_string : public std::vector<weighted_char<Container>, Allocator>
{
    public:

        typedef weighted_char<Container> w_char;

    private:
    
        char _gap = NO_GAP;

    public:

        //! Use basic constructor to allow initialization with bracket such as {el1, el2}
        //! Allow all other std::vector constructors
        using std::vector<weighted_char<Container>, Allocator>::vector;

        void set_gap(char gap)
        {
            _gap = gap;
        }

        bool has_gap() const
        {
            return NO_GAP != gap();
        }

        char gap() const
        {
            return _gap;
        }

        std::string heaviest() const
        {
            return _heaviest(true);
        }

        std::string heaviest_ungap() const
        {
            return _heaviest(false);
        }

    private:

        std::string _heaviest(bool with_gap) const
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
};


//! A class for collection of weighted strings
/*! 
  * \tparam WString     The weighted string type to store in the collection
  * \tparam Collection  Any collection with two template parameters such as STL collection
  * \tparam Allocator   The allocator for the weighted strings
  *
  * This is just a class "in case" we need because some other class will inherit from it
  *
  * \sa wstr::weighted_string
 */
template<
    class WString,
    template <class, class> class Collection = std::vector,
    class Allocator = std::allocator<WString>
>
class weighted_string_collection : public Collection<WString, Allocator>
{
    
};


//! Basic translator for weighted string using a w_array Container
/*!
  * \tparam alph    The string representing the alphabet of the weighted_string
  * 
  * Note that this parameter should be predefined as a static const variable before passing as template argument
  * See https://stackoverflow.com/questions/2033110/passing-a-string-literal-as-a-type-argument-to-a-class-template
  *
  * \sa wstr::weighted_string
  * \sa wstr::w_array
 */ 
template <const char* alph>
struct basic_ws_translator
{
    public:

        //! Return the indice of a letter inside the array
        //! TODO: Find a O(1) way to do this
        static size_t get_indice(char c)
        {
            for (size_t i = 0; i < strlen(alph); ++i) {
                if (alph[i] == c) {
                    return i;
                }
            }

            throw std::runtime_error("character " + std::string(1, c) + " doesn't exists in the alphabet");
        }

        static const char& get_element(size_t i)
        {
            return alph[i];
        }
};

//! weighted char using map
using w_char_map = wc_map<>;

//! weighted string using a map as weighted element
using w_string_map = weighted_string<w_char_map>;

//! weighted string collection for weighted string using map
using w_string_map_collection = weighted_string_collection<w_string_map>;

//! weighted char using an array
/*!
  * \tparam alph    The alphabet of the weighted char
 */
template <const char* alph>
using w_char_array = wc_array<strlen(alph), basic_ws_translator<alph>>;

//! weighted string using an array as weighted element
/*!
  * \tparam alph    The alphabet of the weighted string
 */
template <const char* alph>
using w_string_array = weighted_string<w_char_array<alph>>;

//! weighted collection for weighted string using array
/*!
  * \tparam alph    The alphabet of the weighted collection
 */
template <const char* alph>
using w_string_array_collection = weighted_string_collection<w_string_array<alph>>;


/*! 
  *
  * Functions and classes to allow reading weighted strings from file
  *
 */ 

//! Represents if the weighted elements are strict (= sum of probabilities equals to 1).
inline bool& ws_strict()
{
    static bool strict = true;
    return strict;
}

//! Represents the gap of the weighted elements.
inline bool& ws_gap()
{
    static bool has_gap = false;
    return has_gap;
}

//! Represents the precision of strict probabilities.
inline double& ws_precision()
{
    static double precision = 0.;
    return precision;
}

//! Class to create manipulators with parameters
class ws_manip {};

inline std::istream& operator>>(std::istream& in, const ws_manip& manip)
{
    return in;
}

inline std::istream& ws_strict(std::istream& in)
{
    ws_strict() = true;
    return in;
}

inline std::istream& ws_not_strict(std::istream& in)
{
    ws_strict() = false;
    return in;
}

inline std::istream& ws_gap(std::istream& in)
{
    ws_gap() = true;
    return in;
}

inline std::istream& ws_no_gap(std::istream& in)
{
    ws_gap() = false;
    return in;
}

inline ws_manip ws_set_precision(double p)
{ 
    ws_precision() = p;
    return ws_manip();
}

//! Generic function to create a weighted_string from file
/*!
  * \tparam Container   Same as Container param for weighted_element
 */
template <class Container>
void construct_ws_from_file(std::istream& in, weighted_string<Container>& ws, size_t n, const std::string& alph)
{
    if (ws_gap()) {
        ws.set_gap(alph.back());
    }

    double v;

    for (int i = 0; i < n; ++i) {
        Container wc;

        for (char c : alph) {
            in >> v;
            
            // Avoid adding useless probabilities
            if (0. != v) {
                wc[c] = v;
            }
        }
        
        ws.emplace_back(weighted_char<Container>(wc, ws_strict(), ws_precision()));
    }
}

//! >> operator to read a weighted string from a file
/*!
  * \tparam Container   Same as Container param for weighted_element
 */
template <class Container>
std::istream& operator>>(std::istream& in, weighted_string<Container>& ws)
{
    size_t n;
    std::string alph;

    in >> n;
    in >> alph;

    construct_ws_from_file<Container>(in, ws, n, alph);

    return in;
}

//! >> operator to read a collection of weighted string from a file
/*!
  * \tparam Container   Container parameter of weighted_string
  * \tparam Collection  Any collection with two template parameters such as STL collection
  * \tparam Allocator   The allocator for the weighted strings
 */
template <
    class Container,
    template <class, class> class Collection,
    class Allocator
>
std::istream& operator>>(std::istream& in, Collection<weighted_string<Container>, Allocator>& wsc)
{
    size_t l, n;
    std::string alph;

    in >> l;
    in >> alph;

    wsc.clear();

    for (size_t _ = 0; _ < l; ++_) {
        in >> n;
        weighted_string<Container> ws;

        construct_ws_from_file<Container>(in, ws, n, alph);

        wsc.push_back(ws);
    }

    return in;
}

}