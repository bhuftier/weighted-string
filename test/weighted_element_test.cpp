#include <gtest/gtest.h>

#include <wstr/weighted_element.hpp>

using namespace wstr;
using namespace std;

TEST(WeightedElementTest, BasicConstructor) {
    // Constructor with only one element gives probability one to that element
    weighted_element<char> el1('a');

    EXPECT_EQ(el1.p('a'), 1.0);
    EXPECT_EQ(el1.p('b'), 0.0);
    EXPECT_EQ(el1.heaviest_value(), 'a');
    EXPECT_EQ(el1.heaviest_proba(), 1.0);
    EXPECT_EQ(el1.is_good(), true);

    // We can directly set it with the = constructor
    weighted_element<char> el2 = 'b';

    EXPECT_EQ(el2.p('a'), 0.0);
    EXPECT_EQ(el2.p('b'), 1.0);
    EXPECT_EQ(el2.heaviest_value(), 'b');
    EXPECT_EQ(el2.heaviest_proba(), 1.0);
    EXPECT_EQ(el2.is_good(), true);

    // Constructor with probabilities
    weighted_element<char> el3({{'a', .5}, {'b', .5}});

    EXPECT_EQ(el3.p('a'), .5);
    EXPECT_EQ(el3.p('b'), .5);
    EXPECT_EQ(el3.p('c'), 0);
    EXPECT_TRUE(el3.heaviest_value() == 'a' || el3.heaviest_value() == 'b');
    EXPECT_EQ(el3.heaviest_proba(), .5);
    EXPECT_EQ(el3.is_good(), true);

    // Constructor with probabilities not strict
    weighted_element<char> el4({{'a', .7}, {'b', .2}, {'c', .5}}, false);

    EXPECT_EQ(el4.p('a'), .7);
    EXPECT_EQ(el4.p('b'), .2);
    EXPECT_EQ(el4.p('c'), .5);
    EXPECT_EQ(el4.heaviest_value(), 'a');
    EXPECT_EQ(el4.heaviest_proba(), .7);
    EXPECT_EQ(el4.is_good(), false);

    // Constructor with different precision
    weighted_element<char> el5({{'a', .2}, {'b', .2}, {'c', .5}}, .1);
    EXPECT_EQ(el5.is_good(), true);
    el5.set_precision(.01);
    EXPECT_EQ(el5.is_good(), false);

    weighted_element<char> el6({{'a', .4}, {'b', .2}, {'c', .5}}, .1);
    EXPECT_EQ(el6.is_good(), true);
    el6.set_precision(.01);
    EXPECT_EQ(el6.is_good(), false);
}

TEST(WeightedElementTest, Modification) {
    weighted_element<char> el1 = 'a';
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

TEST(WeightedElementTest, Operators) {
    weighted_element<char> el1 = 'a';

    EXPECT_EQ(el1, 'a');
    EXPECT_EQ(el1, weighted_element<char>('a'));

    el1['b'] = .2;

    EXPECT_NE(el1, 'a');
    EXPECT_EQ(el1, weighted_element<char>({{'a', 1}, {'b', .2}}, false));
}

TEST(WeightedElementTest, AdvancedConstructor) {
    struct vector_hash {
        std::size_t operator()(std::vector<int> const& c) const {
            std::size_t seed = c.size();
            for(auto& i : c) {
                seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

    std::vector<int> values = {1, 2, 3, 4};
    std::vector<int> values2 = {1, 2, 3, 5};

    weighted_element<std::vector<int>, vector_hash> el = values;

    EXPECT_EQ(el.p(values), 1.0);
    EXPECT_EQ(el.p(values2), 0.0);
    EXPECT_EQ(el.heaviest_value(), values);
    EXPECT_EQ(el.heaviest_proba(), 1.0);
    EXPECT_EQ(el.is_good(), true);

    el[values2] = .7;
    el[values] = .2;

    EXPECT_EQ(el.p(values), .2);
    EXPECT_EQ(el.p(values2), .7);
    EXPECT_EQ(el.heaviest_value(), values2);
    EXPECT_EQ(el.heaviest_proba(), .7);
    EXPECT_EQ(el.is_good(), false);
}

TEST(WeightedElementTest, Throws) {
    EXPECT_THROW({
        weighted_element<char>({{'a', 2.}});
    }, std::invalid_argument);
    EXPECT_THROW({
        weighted_element<char>({{'a', .2}});
    }, std::invalid_argument);
    EXPECT_THROW({
        weighted_element<char>({{'a', 2.}, {'b', .8}, {'c', .0001}});
    }, std::invalid_argument);
    EXPECT_THROW({
        weighted_element<char>({{'a', 2.}, {'b', .7}, {'c', .0001}});
    }, std::invalid_argument);
    EXPECT_THROW({
        weighted_element<char>({{'a', .2}, {'b', .699999999999999}}, .1);
    }, std::invalid_argument);
}