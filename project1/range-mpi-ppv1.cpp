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
    
    
    int min=INT_MAX, max=INT_MIN; 
    int min_master=INT_MAX, max_master=INT_MIN;
    

    double t1, t2;
   	int i=0;
   	int arrayPartition;

    
   	MPI_Barrier(MPI_COMM_WORLD); 
		t1 = MPI_Wtime(); 
    
    if (rank == 0)//Master Process
    {	
 	    int *array;
	   	int arraySize;
	    
    	int *partitioned_array;
			


			read_file_part_a(argv[1],&array, &arraySize);
			arrayPartition= arraySize/size ;
    	std::cout<<"Sending Data to Nodes"<<std::endl; 
    	
    	
    	for(i=1; i < size; i++)
    	{	
    		MPI_Send (&arrayPartition,1, MPI_INT, i, 0xAAAA, MPI_COMM_WORLD);
    		MPI_Send ((array+(arrayPartition)*i),arrayPartition, MPI_INT, i, 0xBBBB, MPI_COMM_WORLD);
    	}

    	
		
    	partitioned_array = array;
    	//Master Node will consume the 1st partition of the array, so no need to add to its ptr
    	for(i=0; i < arrayPartition; i++) 
    	{
        if (partitioned_array[i] > max_master)
        	max_master = partitioned_array[i];
        if (partitioned_array[i] < min_master)
        	min_master = partitioned_array[i];
      }
			

    	    	
    	for(i=1; i < size; i++)
    	{			  
    		MPI_Recv (&max,1, MPI_INT, i, 0xFFAA, MPI_COMM_WORLD,&s);
		 	 	MPI_Recv (&min,1, MPI_INT, i, 0xFFFB, MPI_COMM_WORLD,&s);
				//Receive all results and compare them in Master Node
      	if (max > max_master)
      	  max_master = max;
      	if (min < min_master)
      	  min_master = min;
    	}

			//Print the overall range (I used long because 32-bit int isn't enough to store the result)
			//Print the result found by Master Node
		  std::cout << "Master is Finished!" << std::endl;
		  std::cout << "max:" << max_master << std::endl;
		  std::cout << "min:" << min_master << std::endl << std::endl; 
		  
    	std::cout << "Total Range:" << long(max_master)-long(min_master) << std::endl;
			
    }
    
    
    
    else
    {
    	//Every node should receive its piece of data
    	
    	MPI_Recv (&arrayPartition, 1 , MPI_INT, 0, 0xAAAA, MPI_COMM_WORLD, &s);
    	int* partitioned_array = (int*)malloc(sizeof(int) * arrayPartition);
    	
    	
    	MPI_Recv (partitioned_array ,arrayPartition, MPI_INT, 0, 0xBBBB, MPI_COMM_WORLD, &s);

    	//Get the ptr to the corresponding array partition
    	
    	for(i=0; i < arrayPartition; i++) 
    	{
        if (partitioned_array[i]> max)
        	max = partitioned_array[i];
        if (partitioned_array[i] < min)
        	min = partitioned_array[i];
      }

		  
		  //Send results to the Master Node
		  MPI_Send (&max,1, MPI_INT, 0, 0xFFAA, MPI_COMM_WORLD);
		  MPI_Send (&min,1, MPI_INT, 0, 0xFFFB, MPI_COMM_WORLD);
		  
		  
		  std::cout << "Pid "<< rank <<"Finished!" << std::endl;
		  std::cout << "max:" << max << std::endl;
		  std::cout << "min:" << min << std::endl << std::endl; 
		  
    }
    
 
		MPI_Barrier(MPI_COMM_WORLD); 
		t2 = MPI_Wtime();
		if (rank == 0)	  		
			std::cout << "Elapsed time is:"<< t2 - t1 << std::endl; 
    MPI_Finalize();

    return 0;
}
