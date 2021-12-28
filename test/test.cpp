#include "autotest.h"
#include <doctest/doctest.h>

using namespace huff_algo_tests;

TEST_CASE("Test archiving and unarchiving and sizes") {
    for (size_t file_len = 1; file_len < 1000; ++file_len) {
        CHECK(test_arch_unarch(file_len) == true);
    }
}

TEST_CASE("Test empty file") {
    CHECK(test_arch_unarch(0) == true);
}

TEST_CASE("Test tree_node serializing") {
    CHECK(test_tree_node_serialize() == true);
}

TEST_CASE("Test tree_node deserializing") {
    CHECK(test_tree_node_deserialize() == true);
}