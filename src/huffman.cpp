#include "huffman.h"

namespace huffman_algo {
    tree_node::tree_node(tree_node* l, tree_node* r) {
        left_.reset(l);
        right_.reset(r);
        freq_ = 0;
        if (l != nullptr && r != nullptr)
            freq_ = l->freq_ + r->freq_;
        letter_ = '\0';
    }

    tree_node::tree_node(char letter, int freq) {
        letter_ = letter;
        freq_ = freq;
    }

    tree_node* tree_node::get_left_node() const {
        return left_.get();
    }

    tree_node* tree_node::get_right_node() const {
        return right_.get();
    }

    void tree_node::set_left_node(tree_node* nd) {
        left_.reset(nd);
    }
    void tree_node::set_right_node(tree_node* nd) {
        right_.reset(nd);
    }

    int tree_node::get_frequence() const {
        return freq_;
    }

    char tree_node::get_letter() const {
        return letter_;
    }

    void tree_node::serialize(bit_ref_writer& brw) const {
        if (left_ != nullptr) {
            brw.add_bit(1);
            brw.write_char(left_->get_letter());
            left_->serialize(brw);
        } else {
            brw.add_bit(0);
        }
        if (right_ != nullptr) {
            brw.add_bit(1);
            brw.write_char(right_->get_letter());
            right_->serialize(brw);
        } else {
            brw.add_bit(0);
        }
    }

    void tree_node::deserialize(bit_ref_reader& brr) {
        if (brr.get_bit() == true) {
            char c;
            c = brr.read_char();
            set_left_node(new tree_node(c, 0));
            left_->deserialize(brr);
        }
        if (brr.get_bit() == true) {
            char c;
            c = brr.read_char();
            set_right_node(new tree_node(c, 0));
            right_->deserialize(brr);
        }
    }

    huff_tree::huff_tree(std::map<char, int>& freqs) {
        auto compare_nodes = [](tree_node* lhs, tree_node* rhs) {
                return (lhs->get_frequence() > rhs->get_frequence());
            }; 
        std::priority_queue <
            tree_node*,
            std::vector<tree_node*>,
            decltype(compare_nodes)> q (compare_nodes);
        try {
            for (auto &it : freqs) {
                tree_node* nd = new tree_node(it.first, it.second);
                q.push(nd);
            }
            if (q.size() == 1) {
                tree_node* c = new tree_node(q.top(), nullptr);
                q.pop();
                q.push(c);
            }
        } catch (std::bad_alloc& e) {
            while (!q.empty()) {
                delete q.top();
                q.pop();
            }
            throw archive_exception("Error: no free memory.");
        }
        tree_node* l;
        tree_node* r;
        try {
            while (q.size() > 1) {
                l = q.top();
                q.pop();
                r = q.top();
                q.pop();
                q.push(new tree_node(l, r));
            }
            if (q.size() != 0) {
                tree_root_.reset(q.top());
                q.pop();
            } else {
                tree_root_.reset(new tree_node(nullptr, nullptr));
            }
            std::vector<bool> v(0);
            process_codes(v, tree_root_.get());
        } catch (std::bad_alloc& e) {
            delete l;
            delete r;
            while (!q.empty()) {
                delete q.top();
                q.pop();
            }
            throw archive_exception("Error: no free memory.");
        }
    }

    tree_node* huff_tree::get_root() const {
        return tree_root_.get();
    }

    std::vector<bool>& huff_tree::get_code(char c) {
        return codes_[c];
    }

    char huff_tree::deserialize_char(bit_ref_reader& brr) {
        tree_node* find_node = get_root();
        while (!(
            find_node->get_left_node() == nullptr &&
            find_node->get_right_node() == nullptr
        )){
            if (brr.get_bit() == 1) {
                if (find_node->get_right_node() == nullptr) {
                    throw archive_exception("Error: input file is wrong");
                }
                find_node = find_node->get_right_node();
            } else {
                if (find_node->get_left_node() == nullptr) {
                    throw archive_exception("Error: input file is wrong");
                }
                find_node = find_node->get_left_node();
            }
        }
        return find_node->get_letter();
    }

    void huff_tree::process_codes(std::vector<bool>& v, tree_node* nd) {
        if (nd == nullptr)
            return;

        if (nd->get_left_node() == nullptr && nd->get_right_node() == nullptr) {
            codes_[nd->get_letter()] = v;
            return;
        }
        if (nd->get_left_node() != nullptr) {
            std::vector<bool> v_left = v;
            v_left.push_back(0);
            process_codes(v_left, nd->get_left_node());
        }
        
        if (nd->get_right_node() != nullptr) {
            std::vector<bool> v_right = v;
            v_right.push_back(1);
            process_codes(v_right, nd->get_right_node());
        }
    }


    huffman_archiver::huffman_archiver(
        const std::string& in_fn,
        const std::string& out_fn
    ){
        try {
            inp.open(in_fn, std::ios::in);
            inp >> std::noskipws;
        } catch (const std::ifstream::failure& e) {
            throw archive_exception("Error: can't open file " + in_fn);
        }
        try {
            outp.open(out_fn, std::ios::out);
        } catch (const std::ifstream::failure& e) {
            throw archive_exception("Error: can't open file " + out_fn);
        }
    }

    huffman_archiver::huffman_archiver(
        const char* in_fn,
        const char* out_fn
    ){
        try {
            inp.open(in_fn, std::ios::in);
            inp >> std::noskipws;
        } catch (const std::ifstream::failure& e) {
            throw archive_exception(
                "Error: can't open file " + std::string(in_fn));
        }
        try {
            outp.open(out_fn, std::ios::out);
        } catch (const std::ifstream::failure& e) {
            throw archive_exception(
                "Error: can't open file " + std::string(out_fn));
        }
    }

    huffman_archiver::~huffman_archiver() {
        inp.close();
        outp.close();
    }

    void huffman_archiver::archive() {
        std::map<char, int> freqs;

        std::istream_iterator<char> end_;
        for (std::istream_iterator<char> beg_(inp); beg_ != end_; ++beg_) {
            ++freqs[*beg_];
            ++source_len_;
        }
        huff_tree huff_tree_(freqs);

        outp.write((char*)&source_len_, sizeof(size_t));
        bit_ref_writer brw(outp);
        huff_tree_.get_root()->serialize(brw);
        extra_len_ = brw.get_counter() + sizeof(size_t);

        inp.clear();
        inp.seekg(0, inp.beg);
        brw.clear_counter();
        for (std::istream_iterator<char> beg_(inp); beg_ != end_; ++beg_) {
           for (auto c : huff_tree_.get_code(*beg_)) {
               brw.add_bit(c);
           }
        }
        if (brw.get_pos() != 0) {
            for (size_t i = brw.get_pos(); i < CHAR_BIT; ++i) {
                brw.add_bit(1);
            }
        }
        received_len_ = brw.get_counter();
    }

    void huffman_archiver::unarchive() {
        inp.read((char*)&received_len_, sizeof(size_t));

        bit_ref_reader brr(inp);

        huff_tree huff_tree_;
        huff_tree_.get_root()->deserialize(brr);
        extra_len_ = brr.get_byte_counter() + sizeof(size_t);
        
        brr.clear_counter();
        for (size_t i = 0; i < received_len_; ++i) {
            char letter = huff_tree_.deserialize_char(brr);
            outp.write((char*)&letter, sizeof(char));
        }
        source_len_ = brr.get_counter();
    }

    size_t huffman_archiver::get_source_data_size() const {
        return source_len_;
    }

    size_t huffman_archiver::get_received_data_size() const {
        return received_len_;
    }

    size_t huffman_archiver::get_extra_data_size() const {
        return extra_len_;
    }

    bool bit_ref_reader::get_bit() {
        pos_ %= 8;
        if (pos_ == 0) {
            char c = 0;
            rs_.read((char*)&c, sizeof(char));
            bs_ = std::bitset<CHAR_BIT>(c);
        }
        ++bit_cnt_;
        if (bit_cnt_ == CHAR_BIT) {
            ++byte_cnt_;
            bit_cnt_ = 0;
        }
        return bs_[CHAR_BIT - 1 - (pos_++)];
    }

    char bit_ref_reader::read_char() {
        char c = 0;
        for (size_t i = 0; i < CHAR_BIT; ++i) {
            c |= get_bit() << (CHAR_BIT - 1 - i);
        }
        return c;
    }

    size_t bit_ref_reader::get_counter() const {
        return byte_cnt_ + 
        (bit_cnt_ + (CHAR_BIT - pos_) % CHAR_BIT + CHAR_BIT - 1) / CHAR_BIT;
    }

    void bit_ref_reader::clear_counter() {
        bit_cnt_ = 0;
        byte_cnt_ = 0;
    }

    size_t bit_ref_reader::get_bit_counter() const {
        return bit_cnt_;
    }

    size_t bit_ref_reader::get_byte_counter() const {
        return byte_cnt_ + (bit_cnt_ + CHAR_BIT - 1) / CHAR_BIT;
    }

    void bit_ref_writer::add_bit(bool b) {
        bs_ |= b << (CHAR_BIT - pos_ - 1);
        ++bit_cnt_;
        ++pos_;
        if (pos_ == CHAR_BIT) {
            pos_ = 0;
            ws_.write((char*)&bs_, sizeof(char));
            bs_ = 0;
        }
    }

    size_t bit_ref_writer::get_counter() const {
        return (bit_cnt_ + CHAR_BIT - 1) / 8;
    }
    
    size_t bit_ref_writer::get_pos() const {
        return pos_;
    }

    void bit_ref_writer::clear_counter() {
        bit_cnt_ = 0;
    }

    void bit_ref_writer::write_char(char c) {
        for (size_t i = 0; i < CHAR_BIT; ++i) {
            add_bit((c & (1 << (CHAR_BIT - 1 - i))));
        }
    }

    std::ostream& operator<<(std::ostream& out, const archive_exception& mexp) {
        out << mexp.what();
        return out;
    }

}