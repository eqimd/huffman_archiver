#include "autotest.h"
namespace huff_algo_tests {
bool test_arch_unarch(size_t file_len) {
    std::srand(std::time(nullptr));
    std::ofstream outp;
    outp.open("test_inp", std::ios::out);
    for (size_t i = 0; i < file_len; ++i) {
        char c = std::rand() % 255;
        outp.write((char*)&c, sizeof(char));
    }
    outp.close();
    huffman_archiver* arch = new huffman_archiver("test_inp", "test_bin");
    arch->archive();
    size_t arch_src = arch->get_source_data_size();
    size_t arch_rec = arch->get_received_data_size();
    size_t arch_extra = arch->get_extra_data_size();
    delete arch;
    huffman_archiver* unarch = new huffman_archiver("test_bin", "test_final");
    unarch->unarchive();
    size_t unarch_src = unarch->get_source_data_size();
    size_t unarch_rec = unarch->get_received_data_size();
    size_t unarch_extra = unarch->get_extra_data_size();
    delete unarch;
    std::ifstream inp;
    inp.open("test_inp", std::ios::in);
    inp >> std::noskipws;
    char buf1[file_len];
    inp.read(buf1, file_len);
    inp.close();
    std::ifstream inp_final;
    inp_final.open("test_final", std::ios::in);
    inp_final >> std::noskipws;
    char buf2[file_len];
    inp_final.read(buf2, file_len);
    inp_final.close();
    if (arch_src != unarch_rec || 
        arch_rec != unarch_src ||
        arch_extra != unarch_extra) {
        std::cerr << "Sizes don't match.\n";
        return 0;
    }
    for (size_t i = 0; i < file_len; ++i) {
        if (buf1[i] != buf2[i]) {
            std::cerr << "Input and output files don't match.\n";
            return 0;
        }
    }
    return 1;
}

bool test_tree_node_serialize() {
    std::vector<std::pair<std::unique_ptr<tree_node>, std::string>> v;
    
    v.push_back(std::make_pair(
        std::unique_ptr<tree_node>(new tree_node(1, 0)), "100000001000"));
    v.push_back(std::make_pair(
        std::unique_ptr<tree_node>(new tree_node(1, 0)), "010000000100"));
    v.push_back(std::make_pair(
        std::unique_ptr<tree_node>(new tree_node(1, 0)),
                                   "1000000010100000001000"));
    v.push_back(std::make_pair(
        std::unique_ptr<tree_node>(new tree_node(1, 0)),
                                   "0100000001100000001000"));

    v[0].first->set_left_node(new tree_node(1, 0));
    v[1].first->set_right_node(new tree_node(1, 0));

    v[2].first->set_left_node(new tree_node(1, 0));
    v[2].first
        ->get_left_node()->set_right_node(new tree_node(1, 0));
    v[3].first->set_right_node(new tree_node(1, 0));
    v[3].first
        ->get_right_node()->set_left_node(new tree_node(1, 0));
    
    for (size_t i = 3; i < 4; ++i) {
        tree_node* nd = v[i].first.get();
        std::string info = v[i].second;
        std::ofstream outp("test_inp", std::ios::out);
        bit_ref_writer brw(outp);
        nd->serialize(brw);
        while (brw.get_pos() > 0) {
            brw.add_bit(1);
        }
        outp.close();
        std::ifstream inp("test_inp", std::ios::in);
        bit_ref_reader brr(inp);
        for (size_t i = 0; i < info.length(); ++i) {
            bool b = brr.get_bit();
            if(!((info[i] == '1' && b == 1) || (info[i] == '0' && b == 0))) {
                return 0;
                inp.close();
            }
        }
        inp.close();
    }
    return 1;
}

bool test_node_eq(tree_node* l, tree_node* r) {
    if (l == nullptr && r == nullptr)
        return 1;
    else if ((l == nullptr && r != nullptr) || (l != nullptr && r == nullptr)) {
        return 0;
    }

    return (l->get_letter() == r->get_letter() &&
        test_node_eq(l->get_left_node(), r->get_left_node()) && 
        test_node_eq(l->get_right_node(), r->get_right_node()));
}

bool test_tree_node_deserialize() {
    std::vector<std::pair<std::unique_ptr<tree_node>, std::string>> v;
    
    v.push_back(std::make_pair(
        std::unique_ptr<tree_node>(new tree_node(1, 0)), "100000001000"));
    v.push_back(std::make_pair(
        std::unique_ptr<tree_node>(new tree_node(1, 0)), "010000000100"));
    v.push_back(std::make_pair(
        std::unique_ptr<tree_node>(new tree_node(1, 0)),
                                   "1000000010100000001000"));
    v.push_back(std::make_pair(
        std::unique_ptr<tree_node>(new tree_node(1, 0)),
                                   "0100000001100000001000"));

    v[0].first->set_left_node(new tree_node(1, 0));
    v[1].first->set_right_node(new tree_node(1, 0));

    v[2].first->set_left_node(new tree_node(1, 0));
    v[2].first
        ->get_left_node()->set_right_node(new tree_node(1, 0));
    v[3].first->set_right_node(new tree_node(1, 0));
    v[3].first
        ->get_right_node()->set_left_node(new tree_node(1, 0));

    for (size_t i = 0; i < v.size(); ++i) {
        std::ofstream outp("test_inp", std::ios::out);
        bit_ref_writer brw(outp);
        for (auto &c : v[i].second) {
            brw.add_bit(c == '1' ? 1 : 0);
        }
        if (brw.get_pos() != 0) {
            for (size_t j = brw.get_pos(); j < CHAR_BIT; ++j) {
                brw.add_bit(1);
            }
        }
        outp.close();

        std::ifstream inp("test_inp", std::ios::in);
        bit_ref_reader brr(inp);
        std::unique_ptr<tree_node> nd(new tree_node(1, 0));
        nd->deserialize(brr);
        if (!test_node_eq(nd.get(), v[i].first.get())) {
            return 0;
        }
    }

    return 1;
}

}