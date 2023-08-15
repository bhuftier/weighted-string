#include <gtest/gtest.h>

#include "config.h"

#include <wstr/weighted_string.h>

using namespace wstr;

TEST(WeightedStringTest, Basic) {
    w_string ws;

    EXPECT_EQ(ws.size(), 0);
    EXPECT_EQ(ws.heaviest(), "");

    ws.push_back('a');
    
    EXPECT_EQ(ws.size(), 1);
    EXPECT_EQ(ws.heaviest(), "a");

    ws.push_back(w_char({{'a', .2}, {'b', .6}, {'c', .2}}));
    
    EXPECT_EQ(ws.size(), 2);
    EXPECT_EQ(ws.heaviest(), "ab");

    EXPECT_FALSE(ws.has_gap());

    ws.set_gap('-');

    EXPECT_TRUE(ws.has_gap());
    EXPECT_EQ(ws.gap(), '-');

    ws.push_back(w_char({{'a', .4}, {'-', .6}}));

    EXPECT_EQ(ws.heaviest(), "ab-");
    EXPECT_EQ(ws.heaviest_ungap(), "ab");

    EXPECT_EQ(ws[0], 'a');

    ws[0] = 'b';
    
    EXPECT_EQ(ws[0], 'b');
    EXPECT_EQ(ws.heaviest(), "bb-");
    EXPECT_EQ(ws.heaviest_ungap(), "bb");
}

TEST(WeightedStringTest, FromFile) {
    w_string ws;

    TEST_FILE("ws1.txt") >> ws;

    EXPECT_EQ(ws.size(), 4);

    std::vector<std::vector<double>> probas = {
        {.3, .2, .25, .25},
        { 0,  0,  0,   1 },
        {.1, .2, .3,  .4 },
        {.3, .4, .3,   0 }
    };

    std::string alph = "ACGT";

    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            EXPECT_EQ(probas[i][j], ws[i].p(alph[j]));
        }
    }

    EXPECT_EQ(ws.heaviest(), "ATTC");
    EXPECT_EQ(ws.heaviest_ungap(), "ATTC");

    ws.set_gap('T');

    EXPECT_EQ(ws.heaviest(), "ATTC");
    EXPECT_EQ(ws.heaviest_ungap(), "AC");

    ws.set_gap('C');

    EXPECT_EQ(ws.heaviest(), "ATTC");
    EXPECT_EQ(ws.heaviest_ungap(), "ATT");
}

TEST(WeightedStringTest, FromFileGap) {
    w_string ws;

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

TEST(WeightedStringTest, FromFileCollection) {
    ws_collection wsc;

    TEST_FILE("wsc1.txt") >> ws_set_precision(.000000001) >> wsc;

    EXPECT_EQ(wsc.size(), 4);
    EXPECT_EQ(wsc[0].size(), 24);
    EXPECT_EQ(wsc[1].size(), 24);
    EXPECT_EQ(wsc[2].size(), 24);
    EXPECT_EQ(wsc[3].size(), 24);
    EXPECT_EQ(wsc[0].heaviest(), "TACCTGGTTGATTCTGCCAGTAGT");
    EXPECT_EQ(wsc[1].heaviest(), "TACCTGGTTGATTCTGCCAGTAGT");
    EXPECT_EQ(wsc[2].heaviest(), "TACCTGGTTGATCCTGCCAGTAGT");
    EXPECT_EQ(wsc[3].heaviest(), "TACCTGGTTGATCCTGCCAGTAGT");
}

TEST(WeightedStringTest, FromFileCollectionGap) {
    ws_collection wsc;

    TEST_FILE("wsc2.txt") >> ws_gap >> wsc;

    EXPECT_EQ(wsc.size(), 3);
    EXPECT_EQ(wsc[0].gap(), '-');
    EXPECT_EQ(wsc[1].gap(), '-');
    EXPECT_EQ(wsc[2].gap(), '-');
    EXPECT_EQ(wsc[0].size(), 8);
    EXPECT_EQ(wsc[1].size(), 6);
    EXPECT_EQ(wsc[2].size(), 7);
    EXPECT_TRUE(wsc[0].heaviest() == "aab-aab-" || wsc[0].heaviest() == "aab--ab-") << "value is " << wsc[0].heaviest();
    EXPECT_EQ(wsc[1].heaviest(), "ab-aba");
    EXPECT_TRUE(wsc[2].heaviest() == "aba-ba-" || wsc[2].heaviest() == "a-a-ba-") << "value is " << wsc[2].heaviest();
    EXPECT_TRUE(wsc[0].heaviest_ungap() == "aabaab" || wsc[0].heaviest_ungap() == "aabab") << "value is " << wsc[0].heaviest_ungap();
    EXPECT_EQ(wsc[1].heaviest_ungap(), "ababa");
    EXPECT_TRUE(wsc[2].heaviest_ungap() == "ababa" || wsc[2].heaviest_ungap() == "aaba") << "value is " << wsc[2].heaviest_ungap();
}