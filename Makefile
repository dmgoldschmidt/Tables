CFLAGS = -g -Wall -I/home/david/code/include -std=c++11

my_test: my_test.cc Index.h Array.h
	 g++ $(CFLAGS) -o my_test my_test.cc
util.o: util.h util.cc
	g++ $(CFLAGS) -c -o util.o util.cc 
Awk.o:	Awk.h Awk.cc util.o 
	g++ $(CFLAGS) -c -o Awk.o Awk.cc
Table.o: Table.h Table.cc
	g++ $(CFLAGS) -c -o Table.o Table.cc
test_Table: test_Table.cc Table.h Index.h Array.h util.o Awk.o 
	 g++ $(CFLAGS) -o test_Table test_Table.cc util.o Awk.o 
test_Index: Index.cc Index.h Array.h util.h
	    g++ $(CFLAGS) -o test_Index Index.cc util.o
