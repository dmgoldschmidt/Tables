CFLAGS = -g -Wall -I/home/david/code/include -std=c++11

my_test: my_test.cc Index.h Array.h
	 g++ $(CFLAGS) -o my_test my_test.cc 
test_Table: test_Table.cc Table.h Index.h Array.h
	 g++ $(CFLAGS) -o test_Table test_Table.cc util.o  
test_Index: Index.cc Index.h Array.h
	    g++ $(CFLAGS) -o test_Index Index.cc
