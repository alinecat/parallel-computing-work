#include <iostream>
#include <fstream>
#include <vector>
#include "sorting.h"
#include "util.h"
#include <chrono>


int main(int argc, char **argv) {

    keyPair *kpArray;
    int keyPairSize;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    
    read_file_part_b(argv[1],&kpArray, &keyPairSize);


    bucket_sort(kpArray,keyPairSize);
    
    
    int diff = 0;
    int max_consec_diff = 0;
    
    //Open o/p file
    std::ofstream output_file;
    output_file.open (argv[2]);
    
    
    
    for(int i = 1; i < keyPairSize; i++) 
    {	//Check if we are in the same bucket
    	if (kpArray[i].firstKey == kpArray[i-1].firstKey)
    	{	//Calculate a difference
    		diff = kpArray[i].secondKey - kpArray[i-1].secondKey;
    		if (diff > max_consec_diff)
    		{//Replace the max_difference with diff
    			max_consec_diff = diff ;
    		}
    	}
    	else
    	{//If a bucket is completed, print the o/p of that bucket.
    		output_file << max_consec_diff << std::endl ;
    		max_consec_diff = 0;
    	}
    }
    //For the last bucket
    output_file << max_consec_diff << std::endl ;
    output_file.close();
    
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Elapsed time is:" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()/1000000.0 << "s" << std::endl;
    return 0;
}
