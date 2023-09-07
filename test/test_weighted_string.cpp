#include <gtest/gtest.h>

#include "config.h"
#include "types.h"

#include <wstr/weighted_string.hpp>

using namespace wstr;

template <typename T>
class WeightedStringTest : public WstrTest<T>
{
    
};

TYPED_TEST_SUITE(WeightedStringTest, MyTypes);

TYPED_TEST(WeightedStringTest, Basic) {
    using WType = weighted_string<TypeParam>;
    using CType = WeightedStringTest<TypeParam>;

    WType ws;

    EXPECT_EQ(ws.size(), 0);
    EXPECT_EQ(ws.heaviest(), "");

    ws.push_back(CType::el({{'a', 1.}}));
    
    EXPECT_EQ(ws.size(), 1);
    EXPECT_EQ(ws.heaviest(), "a");

    ws.push_back(CType::el({{'a', .2}, {'b', .6}, {'c', .2}}));
    
    EXPECT_EQ(ws.size(), 2);
    EXPECT_EQ(ws.heaviest(), "ab");

    EXPECT_FALSE(ws.has_gap());

    ws.set_gap('-');

    EXPECT_TRUE(ws.has_gap());
    EXPECT_EQ(ws.gap(), '-');

    ws.push_back(CType::el({{'a', .4}, {'-', .6}}));

    EXPECT_EQ(ws.heaviest(), "ab-");
    EXPECT_EQ(ws.heaviest_ungap(), "ab");

    EXPECT_EQ(ws[0], CType::el({{'a', 1.}}));

    ws[0] = CType::el({{'b', 1.}});
    
    EXPECT_EQ(ws[0], CType::el({{'b', 1.}}));
    EXPECT_EQ(ws.heaviest(), "bb-");
    EXPECT_EQ(ws.heaviest_ungap(), "bb");
}

TEST(WeightedStringTest, Constructors) {
    w_string_map ws = {
        w_string_map::w_char({{'a', .2}, {'b', .6}, {'c', .2}}),
        w_string_map::w_char({{'a', .1}, {'b', .3}, {'c', .6}}),
        w_string_map::w_char({{'a', .8}, {'d', .2}}),
        w_string_map::w_char({{'b', 1.}})
    };

    static const char dna[] = "acgt";

    w_string_array<dna> ws2 = {
        w_string_array<dna>::w_char({.2, .4, .2, .2}),
        w_string_array<dna>::w_char({.1, .5, .1, .3}),
        w_string_array<dna>::w_char({.2, .4, .4, 0.}),
    };

    EXPECT_EQ(ws2[0].p('a'), .2);
    EXPECT_EQ(ws2[0].p('c'), .4);
    EXPECT_EQ(ws2[0].p('g'), .2);
    EXPECT_EQ(ws2[0].p('t'), .2);

    EXPECT_EQ(ws2[2].p('a'), .2);
    EXPECT_EQ(ws2[2].p('c'), .4);
    EXPECT_EQ(ws2[2].p('g'), .4);
    EXPECT_EQ(ws2[2].p('t'), .0);
}

TYPED_TEST(WeightedStringTest, Operators) {
    using WType = weighted_string<TypeParam>;
    using CType = WeightedStringTest<TypeParam>;

    WType ws = {
        CType::el({{'a', 1.}})
    };

    WType ws2 = ws;

    EXPECT_EQ(ws2, ws);

    WType ws3(10, CType::el({{'a', 1.}}));

    EXPECT_EQ(ws3.size(), 10);
}

TYPED_TEST(WeightedStringTest, FromFile) {
    using WType = weighted_string<TypeParam>;

    WType ws;

    TEST_FILE("ws1.txt") >> ws;

    EXPECT_EQ(ws.size(), 4);

    std::vector<std::vector<double>> probas = {
        {.3, .2, .25, .25},
        { 0,  0,  0,   1 },
        {.1, .2, .3,  .4 },
        {.3, .4, .3,   0 }
    };

    std::string alph = "abcd";

    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            EXPECT_EQ(probas[i][j], ws[i].p(alph[j]));
        }
    }

    EXPECT_EQ(ws.heaviest(), "addb");
    EXPECT_EQ(ws.heaviest_ungap(), "addb");

    ws.set_gap('d');

    EXPECT_EQ(ws.heaviest(), "addb");
    EXPECT_EQ(ws.heaviest_ungap(), "ab");

    ws.set_gap('b');

    EXPECT_EQ(ws.heaviest(), "addb");
    EXPECT_EQ(ws.heaviest_ungap(), "add");
}

TYPED_TEST(WeightedStringTest, FromFileGap) {
    using WType = weighted_string<TypeParam>;

    WType ws;

    EXPECT_THROW({
        TEST_FILE("ws2.txt") >> ws;
    }, std::invalid_argument);

    TEST_FILE("ws2.txt") >> ws_not_strict >> ws_gap >> ws;

    EXPECT_EQ(ws.size(), 9);
    EXPECT_TRUE(ws.has_gap());
    EXPECT_EQ(ws.gap(), 'j');

    std::vector<std::vector<double>> probas = {
        {.01,         .02,         .03,         .04,         .05,         .06,         .07,          .08,         .09,         0},
        {.001,        .002,        .003,        .004,        .005,        .006,        .007,         .008,        .009,        0},
        {.0001,       .0002,       .0003,       .0004,       .0005,       .0006,       .00007,       .0008,       .0009,       0},
        {.00001,      .00002,      .00003,      .00004,      .00005,      .00006,      .000007,      .00008,      .00009,      0},
        {.000001,     .000002,     .000003,     .000004,     .000005,     .000006,     .0000007,     .000008,     .000009,     0},
        {.0000001,    .0000002,    .0000003,    .0000004,    .0000005,    .0000006,    .00000007,    .0000008,    .0000009,    0},
        {.00000001,   .00000002,   .00000003,   .00000004,   .00000005,   .00000006,   .000000007,   .00000008,   .00000009,   0},
        {.000000001,  .000000002,  .000000003,  .000000004,  .000000005,  .000000006,  .0000000007,  .000000008,  .000000009,  0},
        {.0000000001, .0000000002, .0000000003, .0000000004, .0000000005, .0000000006, .00000000007, .0000000008, .0000000009, 0}
    };

    std::string alph = "abcdefghij";

    for (size_t i = 0; i < 9; ++i) {
        for (size_t j = 0; j < 10; ++j) {
            EXPECT_EQ(ws[i].p(alph[j]), probas[i][j]);
        }

        EXPECT_EQ(ws[i].p(ws.gap()), 0);
    }

    EXPECT_EQ(ws.heaviest(), "iiiiiiiii");

    EXPECT_THROW({
        TEST_FILE("ws2.txt") >> ws_strict >> ws_no_gap >> ws;
    }, std::invalid_argument);
}

TYPED_TEST(WeightedStringTest, FromFileCollection) {
    using WType = std::vector<weighted_string<TypeParam>>;

    WType wsv;

    TEST_FILE("wsv1.txt") >> ws_set_precision(.000000001) >> wsv;

    EXPECT_EQ(wsv.size(), 4);
    EXPECT_EQ(wsv[0].size(), 24);
    EXPECT_EQ(wsv[1].size(), 24);
    EXPECT_EQ(wsv[2].size(), 24);
    EXPECT_EQ(wsv[3].size(), 24);
    EXPECT_EQ(wsv[0].heaviest(), "TACCTGGTTGATTCTGCCAGTAGT");
    EXPECT_EQ(wsv[1].heaviest(), "TACCTGGTTGATTCTGCCAGTAGT");
    EXPECT_EQ(wsv[2].heaviest(), "TACCTGGTTGATCCTGCCAGTAGT");
    EXPECT_EQ(wsv[3].heaviest(), "TACCTGGTTGATCCTGCCAGTAGT");
}

TYPED_TEST(WeightedStringTest, FromFileCollectionGap) {
    using WType = std::vector<weighted_string<TypeParam>>;

    WType wsv;

    TEST_FILE("wsv2.txt") >> ws_gap >> wsv;

    EXPECT_EQ(wsv.size(), 3);
    EXPECT_EQ(wsv[0].gap(), '-');
    EXPECT_EQ(wsv[1].gap(), '-');
    EXPECT_EQ(wsv[2].gap(), '-');
    EXPECT_EQ(wsv[0].size(), 8);
    EXPECT_EQ(wsv[1].size(), 6);
    EXPECT_EQ(wsv[2].size(), 7);
    EXPECT_TRUE(wsv[0].heaviest() == "aab-aab-" || wsv[0].heaviest() == "aab--ab-") << "value is " << wsv[0].heaviest();
    EXPECT_EQ(wsv[1].heaviest(), "ab-aba");
    EXPECT_TRUE(wsv[2].heaviest() == "aba-ba-" || wsv[2].heaviest() == "a-a-ba-") << "value is " << wsv[2].heaviest();
    EXPECT_TRUE(wsv[0].heaviest_ungap() == "aabaab" || wsv[0].heaviest_ungap() == "aabab") << "value is " << wsv[0].heaviest_ungap();
    EXPECT_EQ(wsv[1].heaviest_ungap(), "ababa");
    EXPECT_TRUE(wsv[2].heaviest_ungap() == "ababa" || wsv[2].heaviest_ungap() == "aaba") << "value is " << wsv[2].heaviest_ungap();
}