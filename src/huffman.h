#pragma once

#include <string>
#include <map>
#include <queue>
#include <fstream>
#include <iostream>
#include <bitset>
#include <istream>
#include <iterator>
#include <exception>
#include <climits>
#include <memory>

namespace huffman_algo {
    class bit_ref_reader {
    public:
        bit_ref_reader(std::ifstream& it) : rs_(it) {}
        bool get_bit();
        void move_iter();
        char read_char();
        size_t get_counter() const;
        size_t get_bit_counter() const;
        size_t get_byte_counter() const;
        void clear_counter();
    private:
        std::ifstream& rs_;
        std::bitset<CHAR_BIT> bs_ = std::bitset<CHAR_BIT>();
        size_t bit_cnt_ = 0;
        size_t byte_cnt_ = 0;
        size_t pos_ = 0;
    };

    class bit_ref_writer {
    public:
        bit_ref_writer(std::ofstream& it) : ws_(it) {}
        void add_bit(bool b);
        size_t get_counter() const;
        size_t get_pos() const;
        void clear_counter();
        void write_char(char c);
    private:
        std::ofstream& ws_;
        char bs_ = 0;
        size_t pos_ = 0;
        size_t bit_cnt_ = 0;
    };

    class tree_node {
    public:
        tree_node(tree_node* l, tree_node* r);
        tree_node(char letter, int freq);
        tree_node* get_left_node() const;
        tree_node* get_right_node() const;
        void set_left_node(tree_node* nd);
        void set_right_node(tree_node* nd);
        int get_frequence() const;
        char get_letter() const;
        void serialize(bit_ref_writer& brw) const;
        void deserialize(bit_ref_reader& brr);
    private:
        int freq_ = 0;
        char letter_ = 0;
        std::unique_ptr<tree_node> left_{};
        std::unique_ptr<tree_node> right_{};
    };

    class huff_tree {
    public:
        huff_tree() : tree_root_(new tree_node('\0', 0)) {}
        explicit huff_tree(std::map<char, int>& freqs);
        tree_node* get_root() const;
        void clear();
        std::vector<bool>& get_code(char c);
        char deserialize_char(bit_ref_reader& brr);
    private:
        void process_codes(std::vector<bool>& v, tree_node* nd);
        std::unique_ptr<tree_node> tree_root_;
        std::map<char, std::vector<bool>> codes_;
    };

    class huffman_archiver {
    public:
        explicit huffman_archiver(
            const std::string& in_fn,
            const std::string& out_fn);
        explicit huffman_archiver(
            const char* in_fn,
            const char* out_fn);
        ~huffman_archiver();
        void archive();
        void unarchive();
        size_t get_source_data_size() const;
        size_t get_received_data_size() const;
        size_t get_extra_data_size() const;
    private:
        void close_streams();
        std::ifstream inp;
        std::ofstream outp;
        size_t source_len_ = 0;
        size_t extra_len_ = 0;
        size_t received_len_ = 0;
    };

    class archive_exception : public std::logic_error {
    public:
        archive_exception(const std::string& what_arg) :
            std::logic_error(what_arg) {}
        archive_exception(const char* what_arg) :
            std::logic_error(what_arg) {}
        archive_exception(const std::logic_error& other) :
            std::logic_error(other) {}
    private:
        friend std::ostream& operator<<(
            std::ostream& out,
            const archive_exception& mexc);
    };
}