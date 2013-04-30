CFLAGS = -g -Wall -I/home/david/code/include -std=c++11

test_Index: Index.cc Index.h
	    g++ $(CFLAGS) -o test_Index Index.cc