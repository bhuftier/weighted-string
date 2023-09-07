#include <gtest/gtest.h>

#include "types.h"

#include <wstr/weighted_element.hpp>

using namespace wstr;
using namespace std;

template <typename T>
class WeightedElementTest : public WstrTest<T>
{
    
};

TYPED_TEST_SUITE(WeightedElementTest, MyTypes);

TYPED_TEST(WeightedElementTest, BasicConstructor) {
    using WType = weighted_element<char, TypeParam>;
    using CType = WeightedElementTest<TypeParam>;

    // Test with one element
    WType el1(CType::el({{'a', 1.}}));

    EXPECT_EQ(el1.p('a'), 1.0);
    EXPECT_EQ(el1.p('b'), 0.0);
    EXPECT_EQ(el1.heaviest_value(), 'a');
    EXPECT_EQ(el1.heaviest_proba(), 1.0);
    EXPECT_EQ(el1.is_good(), true);

    // We can directly set it with the = constructor
    WType el2 = CType::el({{'b', 1.}});

    EXPECT_EQ(el2.p('a'), 0.0);
    EXPECT_EQ(el2.p('b'), 1.0);
    EXPECT_EQ(el2.heaviest_value(), 'b');
    EXPECT_EQ(el2.heaviest_proba(), 1.0);
    EXPECT_EQ(el2.is_good(), true);

    // Constructor with probabilities
    WType el3 = CType::el({{'a', .5}, {'b', .5}});

    EXPECT_EQ(el3.p('a'), .5);
    EXPECT_EQ(el3.p('b'), .5);
    EXPECT_EQ(el3.p('c'), 0);
    EXPECT_TRUE(el3.heaviest_value() == 'a' || el3.heaviest_value() == 'b');
    EXPECT_EQ(el3.heaviest_proba(), .5);
    EXPECT_EQ(el3.is_good(), true);

    // Constructor with probabilities not strict
    WType el4(CType::el({{'a', .7}, {'b', .2}, {'c', .5}}), false);

    EXPECT_EQ(el4.p('a'), .7);
    EXPECT_EQ(el4.p('b'), .2);
    EXPECT_EQ(el4.p('c'), .5);
    EXPECT_EQ(el4.heaviest_value(), 'a');
    EXPECT_EQ(el4.heaviest_proba(), .7);
    EXPECT_EQ(el4.is_good(), false);

    // Constructor with different precision
    WType el5(CType::el({{'a', .2}, {'b', .2}, {'c', .5}}), .1);
    EXPECT_EQ(el5.is_good(), false);
    EXPECT_EQ(el5.is_good(.1), true);
    EXPECT_EQ(el5.is_good(.01), false);

    WType el6(CType::el({{'a', .4}, {'b', .2}, {'c', .5}}), .1);
    EXPECT_EQ(el6.is_good(), false);
    EXPECT_EQ(el6.is_good(.1), true);
    EXPECT_EQ(el6.is_good(.01), false);
}

TYPED_TEST(WeightedElementTest, Modification) {
    using WType = weighted_element<char, TypeParam>;
    using CType = WeightedElementTest<TypeParam>;

    WType el1 = CType::el({{'a', 1.}});
    el1['b'] = .2;

    EXPECT_EQ(el1.p('a'), 1.0);
    EXPECT_EQ(el1.p('b'), .2);
    EXPECT_EQ(el1.heaviest_value(), 'a');
    EXPECT_EQ(el1.heaviest_proba(), 1.0);
    EXPECT_EQ(el1.is_good(), false);

    el1['a'] = .2;
    el1['b'] = .8;

    EXPECT_EQ(el1.p('a'), .2);
    EXPECT_EQ(el1.p('b'), .8);
    EXPECT_EQ(el1.heaviest_value(), 'b');
    EXPECT_EQ(el1.heaviest_proba(), .8);
    EXPECT_EQ(el1.is_good(), true);
}

TYPED_TEST(WeightedElementTest, Operators) {
    using WType = weighted_element<char, TypeParam>;
    using CType = WeightedElementTest<TypeParam>;
    
    WType el1 = CType::el({{'a', 1.}});

    EXPECT_EQ(el1, CType::el({{'a', 1.}}));

    el1['b'] = .2;

    EXPECT_NE(el1, CType::el({{'a', 1.}}));
    EXPECT_EQ(el1, WType(CType::el({{'a', 1.}, {'b', .2}}), false));
}

TYPED_TEST(WeightedElementTest, Throws) {
    using WType = weighted_element<char, TypeParam>;
    using CType = WeightedElementTest<TypeParam>;

    EXPECT_THROW({
        WType(CType::el({{'a', 2.}}));
    }, std::invalid_argument);
    EXPECT_THROW({
        WType(CType::el({{'a', .2}}));
    }, std::invalid_argument);
    EXPECT_THROW({
        WType(CType::el({{'a', 2.}, {'b', .8}, {'c', .0001}}));
    }, std::invalid_argument);
    EXPECT_THROW({
        WType(CType::el({{'a', 2.}, {'b', .7}, {'c', .0001}}));
    }, std::invalid_argument);
    EXPECT_THROW({
        WType(CType::el({{'a', .2}, {'b', .699999999999999}}), .1);
    }, std::invalid_argument);
}

TEST(WeightedElementTest, ArraySpecific) {
    struct my_translator {
        static size_t get_indice(char c) {
            if (c == 'a') return 0;
            throw std::runtime_error("");
        }

        static char get_element(size_t i) {
            if (i == 0) return 'a';
            throw std::runtime_error("");
        }
    };

    weighted_element<char, w_array<char, 1, my_translator>> ws = {1.};

    EXPECT_EQ(ws.p('a'), 1.);
    EXPECT_EQ(ws.p('b'), 0.);

    ws['a'] = 1.2;

    EXPECT_EQ(ws.p('a'), 1.2);
    EXPECT_EQ(ws.p('b'), 0.);

    EXPECT_THROW({ws['b'];}, std::runtime_error);
}

std::vector<int> v0 = {1, 2, 3, 4};
std::vector<int> v1 = {1, 2, 3, 5};

template <class T>
void advanced_constructor_test(weighted_element<std::vector<int>, T>& el) {
    el[v0] = 1.;

    EXPECT_EQ(el.p(v0), 1.0);
    EXPECT_EQ(el.p(v1), 0.0);
    EXPECT_EQ(el.heaviest_value(), v0);
    EXPECT_EQ(el.heaviest_proba(), 1.0);
    EXPECT_EQ(el.is_good(), true);

    el[v1] = .7;
    el[v0] = .2;

    EXPECT_EQ(el.p(v0), .2);
    EXPECT_EQ(el.p(v1), .7);
    EXPECT_EQ(el.heaviest_value(), v1);
    EXPECT_EQ(el.heaviest_proba(), .7);
    EXPECT_EQ(el.is_good(), false);
}

TEST(WeightedElementTest, AdvancedConstructorMap) {
    struct vector_hash {
        std::size_t operator()(std::vector<int> const& c) const {
            std::size_t seed = c.size();
            for(auto& i : c) {
                seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

    using TypeParam = w_map<std::vector<int>, vector_hash>;
    using WType = weighted_element<std::vector<int>, TypeParam>;

    WType el = TypeParam({{v0, 1.}});

    advanced_constructor_test(el);
}

TEST(WeightedElementTest, AdvancedConstructorArray) {
    struct vector_translator {
        static size_t get_indice(const std::vector<int>& v) {
            if (v0 == v) { return 0; }
            else         { return 1; }
        }

        static const std::vector<int>& get_element(size_t i) {
            if (0 == i)  { return v0; }
            else         { return v1; }
        }
    };

    using TypeParam = w_array<std::vector<int>, 2, vector_translator>;
    using WType = weighted_element<std::vector<int>, TypeParam>;

    TypeParam t;
    t[v0] = 1.;
    WType el = t;

    advanced_constructor_test(el);
}