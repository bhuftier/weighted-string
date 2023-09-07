#include <gtest/gtest.h>

#include "config.h"

#include <wstr/dna_weighted_string.hpp>

using namespace wstr;

template <class T>
class DnaWeightedStringTest : public testing::Test
{

};

using MyTypes = testing::Types<
    w_string_dna,
    w_string_dna_gap
>;
TYPED_TEST_SUITE(DnaWeightedStringTest, MyTypes);

TEST(DnaWeightedStringTest, Basic) {
    w_string_dna ws;
    w_string_dna_gap ws_gap;

    EXPECT_FALSE(ws.has_gap());
    EXPECT_TRUE(ws_gap.has_gap());
    EXPECT_EQ(ws_gap.gap(), '-');
}

TYPED_TEST(DnaWeightedStringTest, Proba) {
    TypeParam ws;
    typename TypeParam::w_char new_char;

    new_char['A'] = .5;
    new_char['C'] = .2;
    new_char['G'] = .3;

    ws.push_back(new_char);
    
    EXPECT_EQ(ws.size(), 1);
    EXPECT_EQ(ws[0], new_char);
    EXPECT_EQ(ws[0].p('A'), .5);
    EXPECT_EQ(ws[0].p('C'), .2);
    EXPECT_EQ(ws[0].p('G'), .3);
    EXPECT_EQ(ws[0].p('T'), .0);
    EXPECT_EQ(ws[0].p('R'), .8);
    EXPECT_EQ(ws[0].p('Y'), .2);
    EXPECT_EQ(ws[0].p('M'), .7);
    EXPECT_EQ(ws[0].p('K'), .3);
    EXPECT_EQ(ws[0].p('S'), .5);
    EXPECT_EQ(ws[0].p('W'), .5);
    EXPECT_EQ(ws[0].p('H'), .7);
    EXPECT_EQ(ws[0].p('B'), .5);
    EXPECT_EQ(ws[0].p('V'), 1.);
    EXPECT_EQ(ws[0].p('D'), .8);
    EXPECT_EQ(ws[0].p('N'), 1.);
}

TEST(DnaWeightedStringTest, Constructors) {
    w_string_dna ws = {
        w_string_dna::w_char({.2, .4, .2, .2}),
        w_string_dna::w_char({.1, .5, .1, .3}),
        w_string_dna::w_char({.2, .4, .4, 0.}),
    };

    w_string_dna_gap ws2 = {
        w_string_dna_gap::w_char({.2, .4, .2, .1, .1}),
        w_string_dna_gap::w_char({.1, .5, .1, .2, .1}),
        w_string_dna_gap::w_char({.2, .4, .4, 0., 0.}),
    };
}

TYPED_TEST(DnaWeightedStringTest, File) {
    TypeParam ws;
    TEST_FILE("dna1.txt") >> ws;

    std::vector<std::vector<double>> probas = {
        {.2, .25, .25, .3},
        { 0,  0 ,  1 ,  0},
        {.2, .3 , .4 , .1},
        {.4, .3 ,  0 , .3}
    };

    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            EXPECT_EQ(probas[i][j], ws[i].p(dna_alph[j]));
        }
    }
}

TEST(DnaWeightedStringTest, FileGap) {
    w_string_dna ws1;
    w_string_dna_gap ws2;

    TEST_FILE("dna2.txt") >> ws2;

    EXPECT_EQ(ws2.heaviest(), "G-GA");
    EXPECT_EQ(ws2.heaviest_ungap(), "GGA");

    EXPECT_THROW({
        TEST_FILE("dna2.txt") >> ws1;
    }, std::runtime_error);
}

TEST(DnaWeightedStringTest, FileCollectionGap) {
    w_string_dna_collection wsc1;
    w_string_dna_gap_collection wsc2;

    TEST_FILE("dna3.txt") >> wsc2;

    EXPECT_EQ(wsc2[0].gap(), '-');
    EXPECT_EQ(wsc2[0].heaviest(), "G-GA");
    EXPECT_EQ(wsc2[0].heaviest_ungap(), "GGA");

    EXPECT_EQ(wsc2[1].gap(), '-');
    EXPECT_EQ(wsc2[1].heaviest(), "C-");
    EXPECT_EQ(wsc2[1].heaviest_ungap(), "C");

    EXPECT_THROW({
        TEST_FILE("dna3.txt") >> wsc1;
    }, std::runtime_error);
}

TYPED_TEST(DnaWeightedStringTest, CollectionFile) {
    weighted_string_collection<TypeParam> wsv;
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