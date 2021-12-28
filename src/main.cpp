#include "huffman.h"
#include <cstring>

bool check_arguments(const char* arg1, const char* arg2) {
	bool valid_arg1 =
		(strcmp(arg1, "-f") == 0) ||
		(strcmp(arg1, "--file") == 0);
		
	bool valid_arg2 = 
		(strcmp(arg2, "-o") == 0) ||
		(strcmp(arg2, "--output") == 0);
		
    return valid_arg1 && valid_arg2;
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        std::cerr << "Error: invalid arguments.\n";
        return -1;
    }
    try {
        std::string in_fn = "";
        std::string out_fn = "";
        if (check_arguments(argv[2], argv[4])) {
            in_fn = argv[3];
            out_fn = argv[5];
        } else if (check_arguments(argv[4], argv[2])) {
            in_fn = argv[5];
            out_fn = argv[3];
        } else {
            std::cout << argv[2] << " " << argv[4] << std::endl;
            std::cerr << "Error: invalid arguments.\n";
            return -1;
        }
        huffman_algo::huffman_archiver arch(in_fn, out_fn);
        if (strcmp(argv[1], "-c") == 0) {
            arch.archive();
        } else if (strcmp(argv[1], "-u") == 0) {
            arch.unarchive();
        } else {
            std::cerr << "Error: wrong action.\n";
            return -1;
        }
        std::cout << arch.get_source_data_size() << std::endl
                  << arch.get_received_data_size() << std::endl
                  << arch.get_extra_data_size() << std::endl;
    } catch (huffman_algo::archive_exception& e) {
        std::cerr << e << std::endl;
        return -1;
    }
    return 0;
}
