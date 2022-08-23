#include <iostream>
#include <vector>
#include <climits>
#include "util.h"
#include <chrono>

int main(int argc, char **argv) {
    

    
    int min=INT_MAX, max=INT_MIN; 
    
   	int i;
    int *array;
   	int arraySize;
		
		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		
		read_file_part_a(argv[1],&array, &arraySize);

		for(i=0; i < arraySize; i++) 
  	{
      if (array[i]> max)
      	max = array[i];
      if (array[i] < min)
      	min = array[i];
    }
		
	  std::cout << "max:" << max << std::endl;
	  std::cout << "min:" << min << std::endl << std::endl; 
	  
  	std::cout << "Total Range:" << long(max)-long(min) << std::endl;
		
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Elapsed time is:" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()/1000000.0 << "s" << std::endl;	
    return 0;
}
