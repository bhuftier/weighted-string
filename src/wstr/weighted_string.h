#pragma once

#include <vector>

#include "weighted_element.hpp"

#define NO_GAP -1

namespace wstr
{

class weighted_string;

typedef weighted_element<char>  w_char;
typedef weighted_string         w_string;
typedef std::vector<w_string>   ws_collection;

//! A class for weighted strings based on a collection (std::vector) of weighted characters.
/*! 
  * This class is a public inheritance of std::vector for weighted character.
  * It means that you can use all vector method like size(), clear()...
  * You can also have foreach like loop over weighted characters of the weighted string.
  *
  * A weighted string can have a gap character assigned to it.
  *
  * \sa wstr::weighted_element
 */
class weighted_string : public std::vector<w_char>
{
    private:
    
        char _gap = NO_GAP;

    public:

        void set_gap(char gap);

        bool has_gap() const;

        char gap() const;

        std::string heaviest() const;

        std::string heaviest_ungap() const;
    
    private:

        std::string heaviest(bool with_gap) const;

};

/*! 
  * Functions and classes to allow reading weighted strings from file
 */ 

//! Class to create manipulators with parameters
class ws_manip {};

std::istream& operator>>(std::istream& input, const ws_manip& manip);
std::istream& operator>>(std::istream& input, weighted_string& ws);
std::istream& operator>>(std::istream& input, ws_collection& wsv);
std::istream& ws_strict(std::istream& input);
std::istream& ws_not_strict(std::istream& input);
std::istream& ws_gap(std::istream& input);
std::istream& ws_no_gap(std::istream& input);
ws_manip ws_set_precision(double p);

}