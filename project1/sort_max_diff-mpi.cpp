#include <iostream>
#include <fstream>
#include <vector>
#include "sorting.h"
#include "util.h"
#include "mpi.h"

void find_max_consecutive_diff(keyPair kpArray[], int kpSize, int result_arr[]);

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
    
    double t1, t2;
    int i=0;

    
   	MPI_Barrier(MPI_COMM_WORLD); 
		t1 = MPI_Wtime(); 
    
    if (rank == 0)//Master Process
    {	
		  keyPair *kpArray;
			int keyPairSize;
			
			read_file_part_b(argv[1],&kpArray, &keyPairSize);
			
			
			int min=INT_MAX, max=INT_MIN; 
			
			//Find the range of bucket ids
			for(int i = 0; i < keyPairSize; i++) 
			{
				if (max < *((&kpArray->firstKey)+(2*i)))
					max = *((&kpArray->firstKey)+(2*i));
				if (min > *((&kpArray->firstKey)+(2*i)))
					min = *((&kpArray->firstKey)+(2*i));
			}		

		  int overall_range = max - min ;
			int buck_start, buck_end, buck_cnt_per_proc;
			
			buck_cnt_per_proc = (overall_range+1)/size;
			
 
			for(i=1; i < size; i++)
    	{	
    		//Partition the workload
    		buck_start = min + buck_cnt_per_proc*i;
				buck_end = min + buck_cnt_per_proc*(i+1)-1;
				
    		MPI_Send (&buck_start,1, MPI_INT, i, 2, MPI_COMM_WORLD);
    		MPI_Send (&buck_end,1, MPI_INT, i, 3, MPI_COMM_WORLD);
    		
    		//Multiplication with 2 comes from the struct (two integers)
    		MPI_Send (&keyPairSize,1, MPI_INT, i, 0xAAAA, MPI_COMM_WORLD);
    		MPI_Send (kpArray, keyPairSize*2 , MPI_INT, i, 0xBBBB, MPI_COMM_WORLD);
    	}
    	 	
    	// Rank = 0 for master process
    	buck_start = min + buck_cnt_per_proc*rank;
			buck_end = min + buck_cnt_per_proc*(rank+1)-1; 	
			
			
			keyPair*  partial_kpArray = (keyPair*)malloc(sizeof(keyPair) * (keyPairSize/size)) ;
			
			partial_bucket_sort(kpArray, partial_kpArray, keyPairSize, buck_start, buck_end);
			
			
			int part_keyPairSize = keyPairSize/size;
			
			
			int* consec_diffs = (int*)malloc(sizeof(int) * (buck_cnt_per_proc));
    	
    	find_max_consecutive_diff(partial_kpArray , part_keyPairSize , consec_diffs) ;
      
     
      
      std::ofstream output_file;
      output_file.open (argv[2]);
      
      for(int i = 0; i < buck_cnt_per_proc ; i++) 
		     output_file << consec_diffs[i]  << std::endl;
		  
			
			

			for(i=1; i < size; i++)
    	{
    		MPI_Recv (consec_diffs, buck_cnt_per_proc , MPI_INT, i, 4, MPI_COMM_WORLD, &s);
    		for(int i = 0; i < buck_cnt_per_proc ; i++) 
		    	output_file << consec_diffs[i] << std::endl;
			}
			output_file.close();
    }
    
    
    
    else
    {
    	int keyPairSize;
    	int part_keyPairSize;
    	int bucket_id_start,bucket_id_end;
    	
    	//Receive the workload, i.e. the buckets from start to end
  		MPI_Recv (&bucket_id_start,1, MPI_INT, 0, 2, MPI_COMM_WORLD, &s);
  		MPI_Recv (&bucket_id_end,1, MPI_INT, 0, 3, MPI_COMM_WORLD, &s);
   		
   		MPI_Recv (&keyPairSize, 1 , MPI_INT, 0, 0xAAAA, MPI_COMM_WORLD, &s);
    	keyPair* kpArray = (keyPair*)malloc(sizeof(keyPair) * keyPairSize);
    	
    	//Multiplication with 2 comes from the struct (two integers)
    	
    	MPI_Recv (kpArray, keyPairSize*2, MPI_INT, 0, 0xBBBB, MPI_COMM_WORLD, &s);
    	
    	//Partition keyPair array size
    	part_keyPairSize = keyPairSize/size;
    	
    	keyPair*  partial_kpArray = (keyPair*)malloc(sizeof(keyPair) * part_keyPairSize) ;
    	
    	//Sort the values into partial_kpArray, read data from kpArray 
    	partial_bucket_sort(kpArray, partial_kpArray, keyPairSize, bucket_id_start, bucket_id_end);
    	
    	
    	//Generate a result array
    	int* consec_diffs = (int*)malloc(sizeof(int) * (bucket_id_end-bucket_id_start+1));
    	
    	
    	find_max_consecutive_diff(partial_kpArray , part_keyPairSize , consec_diffs) ;
    	
 			//Send the results to master node
			MPI_Send (consec_diffs, (bucket_id_end-bucket_id_start+1) , MPI_INT, 0, 4, MPI_COMM_WORLD);
    }
    
 
		MPI_Barrier(MPI_COMM_WORLD); 
		t2 = MPI_Wtime();
		if (rank == 0)	  		
			std::cout << "Elapsed time is:"<< t2 - t1 << std::endl; 
    MPI_Finalize();
    return 0;
}



void find_max_consecutive_diff(keyPair kpArray[], int kpSize, int result_arr[])
{
	
  int diff = 0;
  int max_consec_diff = 0;
  int result_idx = 0;
 
 	//Find the consecutive differences in a kpArray
  for(int i = 1; i < kpSize; i++) 
  {
  	 
  	if (kpArray[i].firstKey == kpArray[i-1].firstKey)
  	{	
  		diff = kpArray[i].secondKey - kpArray[i-1].secondKey;
  		if (diff > max_consec_diff)
  		{
  			max_consec_diff = diff;
  		}
  	}
  	else
  	{
			result_arr[result_idx] = max_consec_diff;
  		max_consec_diff = 0;
  		result_idx++;
  	}
  }
  //For the final bucket
  result_arr[result_idx] = max_consec_diff;

}
