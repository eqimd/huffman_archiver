#pragma once

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <ctime>
#include "huffman.h"

using namespace huffman_algo;

namespace huff_algo_tests {
    bool test_arch_unarch(size_t file_len);
    bool test_tree_node_serialize();
    bool test_node_eq(tree_node* l, tree_node* r);
    bool test_tree_node_deserialize();
}