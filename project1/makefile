compile:
	g++ range-serial.cpp util.h util.cpp -o range-serial
	mpiCC util.h util.cpp range-mpi-ppv1.cpp -o range-mpi-ppv1
	mpiCC util.h util.cpp range-mpi-ppv2.cpp -o range-mpi-ppv2
	g++ sort_max_diff-serial.cpp sorting.h util.h util.cpp -o sort_max_diff-serial
	mpiCC util.h sorting.h util.cpp sort_max_diff-mpi.cpp -o sort_max_diff-mpi


