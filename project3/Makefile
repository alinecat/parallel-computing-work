all: kdtree_serial kdtree_parallel

kdtree_perf: util.o src/kdtree_serial.cpp
	g++ -fno-omit-frame-pointer -o kdtree_perf src/kdtree_serial.cpp util.o

kdtree_serial: util.o src/kdtree_serial.cpp
	g++ -g -o kdtree_serial src/kdtree_serial.cpp util.o

kdtree_parallel: util.o src/kdtree_parallel.cpp
	g++ -fopenmp -o kdtree_parallel src/kdtree_parallel.cpp util.o

util.o: src/util.c src/util.h
	g++ -c src/util.c
    

check_serial_parallel: 
	./kdtree_parallel dim5/sample_points_small.txt dim5/sample_queries_small.txt out_p.txt
	./kdtree_serial dim5/sample_points_small.txt dim5/sample_queries_small.txt out_s.txt
	diff out_p.txt out_s.txt


.PHONY: clean
clean:
	rm util.o kdtree_serial kdtree_parallel kdtree_perf
