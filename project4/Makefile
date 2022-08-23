NVCC=nvcc
CUDAFLAGS= -std=c++14 -I .

k_mer_parallel: k_mer_parallel.cu util.cu
	$(NVCC) $(CUDAFLAGS) -o k_mer_p k_mer_parallel.cu util.cu


	
k_mer_serial: k_mer_serial.cu util.cu
	$(NVCC) $(CUDAFLAGS) -o k_mer_s k_mer_serial.cu util.cu
	
clean:
	rm -rf kmer *.o
