#include <gtest/gtest.h>

#include "config.h"

#include <wstr/weighted_string.h>

using namespace wstr;

TEST(ReadmeExampleTest, OptionGaps) {
    w_string ws;
    ws.set_gap('-');

    EXPECT_EQ(ws.gap(), '-');

    std::ifstream input = TEST_FILE("readme_gaps");
    input >> ws_gap >> ws;

    EXPECT_EQ(ws.gap(), '-');
}

TEST(ReadmeExampleTest, OptionStrict) {
    EXPECT_NO_THROW({
        w_char wc({{'a', .2}, {'b', .4}}, false);
    });

    EXPECT_THROW({
        w_char wc({{'a', .2}, {'b', .4}});
    }, std::invalid_argument);

    EXPECT_NO_THROW({
        w_string ws;
        std::ifstream input = TEST_FILE("readme_strict");
        input >> ws_not_strict >> ws;
    });

    EXPECT_THROW({
        w_string ws;
        std::ifstream input = TEST_FILE("readme_strict");
        input >> ws_strict >> ws;
    }, std::invalid_argument);
}

TEST(ReadmeExampleTest, OptionPrecision) {
    EXPECT_NO_THROW({
        w_char wc1({{'a', .4}, {'b', .5}}, .1);
    });

    EXPECT_THROW({
        w_char wc1({{'a', .4}, {'b', .5}});
    }, std::invalid_argument);

    w_char wc2({{'a', .5}, {'b', .5}});
    wc2['a'] = .4;

    EXPECT_FALSE(wc2.is_good());

    wc2.set_precision(.1);
    wc2.is_good();

    EXPECT_TRUE(wc2.is_good());

    EXPECT_NO_THROW({
        w_string ws;
        std::ifstream input = TEST_FILE("readme_precision");
        input >> ws_set_precision(0.00001) >> ws;
    });

    EXPECT_THROW({
        w_string ws;
        std::ifstream input = TEST_FILE("readme_precision");
        input >> ws_set_precision(0.) >> ws;
    }, std::invalid_argument);
}