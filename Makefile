CFLAGS = -g -Wall -I/home/david/code/include -std=c++11

my_test: my_test.cc Index.h Array.h
	 g++ $(CFLAGS) -o my_test my_test.cc
util.o: util.h util.cc
	g++ $(CFLAGS) -c -o util.o util.cc 
GetOpt.o: GetOpt.h GetOpt.cc
	g++ $(CFLAGS) -c -o GetOpt.o GetOpt.cc
Awk.o:	Awk.h Awk.cc util.o 
	g++ $(CFLAGS) -c -o Awk.o Awk.cc
Table.o: Table.h Table.cc
	g++ $(CFLAGS) -c -o Table.o Table.cc
test_Table: test_Table.cc Table.h Index.h Array.h util.o Awk.o Table.o
	 g++ $(CFLAGS) -o test_Table test_Table.cc util.o Awk.o Table.o
test_Index: Index.cc Index.h Array.h util.h
	g++ $(CFLAGS) -o test_Index Index.cc util.o
matrix.o: matrix.cc Matrix.h
	g++ $(CFLAGS) -c matrix.cc
SparseMatrix.o: Matrix.h SparseMatrix.cc
	g++ $(CFLAGS) -c SparseMatrix.cc
testSparse: testSparse.cc SparseMatrix.o Matrix.h util.o matrix.o
	g++ $(CFLAGS) -o testSparse testSparse.cc SparseMatrix.o util.o matrix.o
recommend: recommend.cc Matrix.h SparseMatrix.o Awk.o  matrix.o Matrix.h util.o GetOpt.o 
	g++ $(CFLAGS) -o recommend recommend.cc Awk.o util.o matrix.o SparseMatrix.o GetOpt.o
Recommender.o: Recommender.cc Recommender.h
	g++ $(CFLAGS) -c Recommender.cc
test_Recommender: test_Recommender.cc Histogram.h Recommender.o Matrix.h SparseMatrix.o Awk.o matrix.o Matrix.h util.o GetOpt.o
	g++ $(CFLAGS) -o test_Recommender test_Recommender.cc Recommender.o Awk.o util.o matrix.o SparseMatrix.o GetOpt.o
test_Histogram: test_Histogram.cc Histogram.h Array.h
	g++ $(CFLAGS) -o test_Histogram test_Histogram.cc
testArray: testArray.cc Array.h util.o
	g++ $(CFLAGS) -o testArray testArray.cc util.o