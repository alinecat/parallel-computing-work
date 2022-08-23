//Skeleton SSSP serial

#include <iostream>
#include "util.h"
#include <bits/stdc++.h>
#include <chrono>
int min(int arr[], int arr_size);

int main(int argc, char **argv) {
	if (argc != 4)
	{
		std::cout << "Wrong argments usage: sssp_serial [DATA_FILE] [sourceVertex] [OUTPUT_FILE]" << std::endl;
	}

	int numVertices, numEdges, *vertices, *edges, *weights;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	//Reading the input
	int success = read_file(argv[1], &numVertices, &numEdges, &vertices);
	if (success) {
		edges = &(vertices[numVertices + 1]);
		weights = &(vertices[numVertices + 1 + numEdges]);
		
		int R[numVertices], D[numVertices];	
		int sourceVertex = std::stoi(argv[2]);  
        int *resultDistances = new int[numVertices];
		
        for (int x=0; x<numVertices; x++)					//Set D and R to infinity
        {
        	R[x] = INT_MAX;
        	D[x] = INT_MAX;
        }
        
        D[sourceVertex] = 0;								//Set the distance from itself to 0
        
		/////////////////////////////////////////////////
		//         DO YOUR COMPUTATIONS      ////////////
		/////////////////////////////////////////////////
		int a = 0, i, j, k;
		bool R_D_equal = true;
		int iter_cnt = 0;
        for(j = 0; j<numVertices-1; j++)
        {
        	iter_cnt++;
        	
        	for (i=0; i<numVertices ; i++)
        	{
        		a = INT_MAX;
        		for (k=vertices[i]; k<vertices[i+1]; k++)	//Multiply a row 
        		{	
        			//If infinity, prevent overflow
        			if ((D[edges[k]] != INT_MAX )&((weights[k] + D[edges[k]]) < a))//If there is a smaller value in corresponding row, set it to "a"
        				a = weights[k] + D[edges[k]];
        		}
        		R[i] = std::min(D[i], a);					//To ensure the distance from itself is 0
        	}
        	
        	for (int y=0; y < numVertices; y++)
        	{	
        		//If there is an inequality, break and set the equality boolean to false
        		if (D[y] != R[y])
        		{
        			R_D_equal = false;
        			break;
        		}
        	}
        	//If the results are the same for R and D, break the loop
        	if (R_D_equal == true)
        		break;
        	R_D_equal = true;
        	for (int y=0; y < numVertices; y++)
        	{	
        		D[y] = R[y];
        	}
        	
        }
        
        
    	for (int y=0; y < numVertices; y++)
    	{
    		resultDistances[y] = D[y];
    	}
    	std::cout << "Total iteration count is:"<< iter_cnt<< std::endl;
        
        //Outputting the results
		printResults(argv[3], resultDistances, numVertices);

		//Free the memory
		delete vertices;
		delete resultDistances;
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Elapsed time is:" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()/1000000.0 << "s" << std::endl;
	return 0;

}
