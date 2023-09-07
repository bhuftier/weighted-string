#include <gtest/gtest.h>

#include "types.h"

#include <wstr/weighted_char.hpp>

using namespace wstr;
using namespace std;

template <typename T>
class WeightedCharTest : public WstrTest<T>
{
    
};

TYPED_TEST_SUITE(WeightedCharTest, MyTypes);

TYPED_TEST(WeightedCharTest, BasicConstructor) {
    using WType = weighted_char<TypeParam>;
    using CType = WeightedCharTest<TypeParam>;

    // Test with one element
    WType el1(CType::el({{'a', .6}, {'b', .4}}));

    EXPECT_EQ(el1.p('a'), .6);
    EXPECT_EQ(el1.p('b'), .4);
    EXPECT_EQ(el1.heaviest_value(), 'a');
    EXPECT_EQ(el1.heaviest_proba(), .6);
    EXPECT_EQ(el1.heaviest_non_gap_value('a'), 'b');
    EXPECT_EQ(el1.heaviest_non_gap_proba('a'), .4);

    // Test with the first element of the array (previous bug)
    WType el2(CType::el({{'A', .4}, {'C', .2}, {'G', .4}}));

    EXPECT_EQ(el2.heaviest_non_gap_value('G'), 'A');
    EXPECT_EQ(el2.heaviest_non_gap_proba('G'), .4);
}