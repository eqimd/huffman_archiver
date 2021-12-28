.PHONY: all clean test

all: huffman_archiver
test: huffman_archiver_test

CFLAGS= -Itest -Isrc -Wall -Werror -g -std=c++14

obj:
	mkdir obj

huffman_archiver: obj obj/huffman.o obj obj obj/main.o
	g++ obj/main.o obj/huffman.o -o huffman_archiver

obj/huffman.o: src/huffman.cpp src/huffman.h
	g++ $(CFLAGS) -c src/huffman.cpp -o obj/huffman.o

obj/main.o: src/main.cpp
	g++ $(CFLAGS) -c src/main.cpp -o obj/main.o


huffman_archiver_test: obj obj/huffman.o obj obj/huffman_test.o obj obj/autotest.o obj/test.o
	g++ obj/test.o obj/huffman.o obj/huffman_test.o obj/autotest.o -o hw_02_test

obj/test.o: test/test.cpp
	g++ $(CFLAGS) -c test/test.cpp -o obj/test.o

obj/autotest.o: test/autotest.cpp test/autotest.h
	g++ $(CFLAGS) -c test/autotest.cpp -o obj/autotest.o

obj/huffman_test.o: test/huffman_test.cpp test/huffman_test.h
	g++ $(CFLAGS) -c test/huffman_test.cpp -o obj/huffman_test.o

clean:
	rm -rf ./obj huffman_archiver huffman_archiver_test test_*
