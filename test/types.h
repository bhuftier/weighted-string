#pragma once

#include <wstr/weighted_string.hpp>

using namespace wstr;

//! Class used for test weighted element and strings with different container
template <typename T>
class WstrTest : public testing::Test
{
    public:

        //! Method to create an element via a map
        static T el(const w_char_map& u) { return T(u); }
};

//! Alphabet used in our test
static const char test_alphabet[] = "ACGTabcdefghij-";

//! Specify the creation of an element when we use a w_array instead of a w_map
template<>
inline w_char_array<test_alphabet> WstrTest<w_char_array<test_alphabet>>::el(const w_char_map& u)
{
    w_char_array<test_alphabet> new_el;

    for (size_t i = 0; i < strlen(test_alphabet); ++i) {
        char c = test_alphabet[i];
        new_el[c] = u.at(c);
    }

    return new_el;
}

//! Container type to test
using MyTypes = testing::Types<
    w_char_map,
    w_char_array<test_alphabet>
>;