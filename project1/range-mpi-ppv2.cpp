#include <iostream>
#include <vector>
#include <climits>
#include "mpi.h"
#include "util.h"


int main(int argc, char **argv) {
    
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
	
		MPI_Status s;			
					
    // Get the number of processes
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    
    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int i=0;
    
    int arrayPartition;
    int min=INT_MAX, max=INT_MIN; 
        
    
    double t1, t2;
   
   	//We should use two reduction operations, maximum and minimum
   	int reduct_max, reduct_min;


    
   	MPI_Barrier(MPI_COMM_WORLD); 
		t1 = MPI_Wtime(); 
    
    if (rank == 0)//Master Process
    {
    	int *array;
    	int arraySize;
    	std::cout<<"Sending Data to Nodes"<<std::endl;
    	    
    	read_file_part_a(argv[1],&array, &arraySize);
   
			arrayPartition= arraySize/size ;
    	//Send data to the other Nodes
    	for(i=1; i < size; i++)
    	{	
    		MPI_Send (&arrayPartition,1, MPI_INT, i, 0xAAAA, MPI_COMM_WORLD);
    		MPI_Send ((array+(arrayPartition)*i),arrayPartition, MPI_INT, i, 0xBBBB, MPI_COMM_WORLD);
    	}
    	
    	int *partitioned_array;
    	partitioned_array = (array+(arrayPartition)*rank);
    	
    	//Master Node will consume the 1st partition of the array, so no need to add to its ptr
    	for(i=0; i < arrayPartition; i++) 
    	{
        if (partitioned_array[i] > max)
        	max = partitioned_array[i];
        if (partitioned_array[i] < min)
        	min = partitioned_array[i];
      }

			//Print the result found by Master Node
		  std::cout << "max:" << max << std::endl;
		  std::cout << "min:" << min << std::endl; 
			std::cout << "Master is Finished!" << std::endl;
    }
    
    
    
    else
    {
    	
    	//Every node should receive its piece of data
    	MPI_Recv (&arrayPartition, 1 , MPI_INT, 0, 0xAAAA, MPI_COMM_WORLD, &s);
    	int* partitioned_array = (int*)malloc(sizeof(int) * arrayPartition);
    	
    	MPI_Recv (partitioned_array ,arrayPartition, MPI_INT, 0, 0xBBBB, MPI_COMM_WORLD, &s);
    	
    	
    	for(i=0; i < arrayPartition; i++) 
    	{
        if (partitioned_array[i]> max)
        	max = partitioned_array[i];
        if (partitioned_array[i] < min)
        	min = partitioned_array[i];
      }
      
      //Each Node prepares(sends) results for Allreduce

      
    	std::cout << "Pid"<< rank <<" Finished!" << std::endl;
		  std::cout << "max:" << max << std::endl;
		  std::cout << "min:" << min << std::endl << std::endl; 
		  

		  
    }
    
		//Independent of Node ID, Allreduce operation will be applied to every Node
	  //There will be two reduction, max and min
		MPI_Allreduce(&max, &reduct_max, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
		MPI_Allreduce(&min, &reduct_min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
		
  	MPI_Barrier(MPI_COMM_WORLD); 

  	t2 = MPI_Wtime(); 
  	if (rank == 0)
  	{
  		//Print the overall range (I used long because 32-bit int isn't enough to store the result)
  		//Print the range by using Master Node
  		std::cout << "Range:" << long(reduct_max)-long(reduct_min) << std::endl;
    	//Get wall time of Master Node
    	std::cout << "Elapsed time is:"<< t2 - t1 << std::endl; 
    }
    MPI_Finalize();
    return 0;
}
