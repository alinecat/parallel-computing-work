//Skeleton SSSP parallel


#include <iostream>
#include "util.h"
#include <bits/stdc++.h>
#include <mpi.h>


int main(int argc, char **argv)
{
	// Initialize the MPI environment
	MPI_Init(&argc, &argv);

	if (argc != 4)
	{
		std::cout << "Wrong argments usage: mpirun -np X sssp_parallel [DATA_FILE] [sourceVertex] [OUTPUT_FILE]" << std::endl;
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	}
	
	
	double t1, t2 ;
	int sourceVertex = std::stoi(argv[2]);

	
	int numVertices, numEdges;
	int arr_partition; //For dividing the arrays
	int *vertices, *edges, *weights;
	
	
	int *resultDistances;


	// Get the rank of the calling process
	int world_rank, world_size;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	
	int Graph_Dim[2]; //For Bcasting graph information
	
	
	t1 = MPI_Wtime();
	//Read the file
	if (world_rank == 0)
	{
		
		int success = read_file(argv[1], &numVertices, &numEdges, &vertices);
		if (success)
		{
			edges = &(vertices[numVertices + 1]);
			weights = &(vertices[numVertices + 1 + numEdges]);
		}
		else
		{
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		}
		
		
		Graph_Dim[0] = numVertices; 
		Graph_Dim[1] = numEdges; 
	}
	
	//Broadcast the graph information	
	MPI_Bcast(Graph_Dim, 2, MPI_INT, 0, MPI_COMM_WORLD);
	
	
	
	numVertices = Graph_Dim[0];
	arr_partition = Graph_Dim[0]/world_size; 
	int vertice_partition[arr_partition+1]; //Buffer for vertices
	int displacements[world_size]; //Displacement is the start idx for each node in scatter
	int partition_size[world_size]; //Number of elements each process gets for each node in scatter
	
	//Generate the displacement and partition arrays for scattering
	if (world_rank == 0)
	{	
		arr_partition = Graph_Dim[0]/world_size; //Number of vertices per processor
		for (int i=0; i < world_size; i++)
		{	
			displacements[i] = arr_partition*i; //Start ofverices for each processor
			partition_size[i] = arr_partition+1; //
			
		}
	}
	//Scatter the vertşces from process 0 to all processes
	MPI_Scatterv(vertices, partition_size, displacements, MPI_INT, vertice_partition, arr_partition+1 , MPI_INT, 0, MPI_COMM_WORLD);
	
	
	//Similar to the code above but, the sizes of edge budder and weight buffer are generated according to the vertice parittion (i.e. the rows)
	int edge_partition[vertice_partition[arr_partition]-vertice_partition[0]];
	int weight_partition[vertice_partition[arr_partition]-vertice_partition[0]];
	if (world_rank == 0)
	{	
		
		for (int i=0; i < world_size; i++)
		{
			displacements[i] = vertices[arr_partition*i];
			partition_size[i] = vertices[arr_partition*(i+1)]-vertices[arr_partition*i];
		}
		
	}
	
	//Broadcast displacements and partition size to all other processes for correct transmission
	MPI_Bcast(displacements, world_size, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(partition_size, world_size , MPI_INT, 0, MPI_COMM_WORLD);

	//Scatter the edges and vertices as anticipated
	MPI_Scatterv(edges, partition_size, displacements, MPI_INT, edge_partition, partition_size[world_rank], MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatterv(weights, partition_size, displacements, MPI_INT, weight_partition, partition_size[world_rank], MPI_INT, 0, MPI_COMM_WORLD);

	
	
	numVertices = Graph_Dim[0];
	numEdges = Graph_Dim[1];
	arr_partition = numVertices/world_size;
	int R_loc[arr_partition];
	int D[numVertices], R[numVertices];	
	int iter_cnt = 0;

	//Set D and R to infinity
	for (int x=0; x<numVertices; x++)					
	{
		D[x] = INT_MAX;
		R[x] = INT_MAX;
	}
	//Set sourceVertex idx of D to 0 (distance from itself is 0)
	D[sourceVertex] = 0;
	//Fill local result array with infinity
	for (int x=0; x<arr_partition; x++)
	{
		R_loc[x] = INT_MAX;
	}
    
	int a = 0, i, j, k;
	//For early termination
	bool R_D_equal = true;
	
	//Parallelized algorithm
	for(j = 0; j<numVertices-1; j++)
	{
		iter_cnt++;
		for (i=0; i<arr_partition ; i++)
		{
			a = INT_MAX;
			for (k=(vertice_partition[i]-vertice_partition[0]); k<(vertice_partition[i+1]-vertice_partition[0]); k++)	//Multiply a row 
			{	
				//If infinity, prevent overflow, find the minimum in a row, each process search on different parition of a row
				if ((D[edge_partition[k]] != INT_MAX )&((weight_partition[k] + D[edge_partition[k]]) < a))//If there is a smaller value in corresponding row, set it to "a"
					a = weight_partition[k] + D[edge_partition[k]];
			}
			R_loc[i] = std::min(D[arr_partition*world_rank+i], a);		  //To ensure the distance from itself is 0, copy the result to R_loc
		}
		
		//Combine R_loc in the R buffer
		MPI_Allgather(R_loc, arr_partition, MPI_INT, R, arr_partition, MPI_INT, MPI_COMM_WORLD);
    	
		
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
    		D[y] = R[y];
    	
	}

	//Outputting the results
	if (world_rank == 0)
	{
	    resultDistances = new int[numVertices];
		for (int y=0; y < numVertices; y++)
		{
			resultDistances[y] = D[y];
		}
		printResults(argv[3], resultDistances, numVertices);
		
		//free up the memory
	    delete vertices;
	    delete resultDistances;
	}

	
	t2 = MPI_Wtime();
	if (world_rank == 0){	  		
		std::cout << "Elapsed time is:"<< t2 - t1 << std::endl;
		std::cout << "Total iteration count is:"<< iter_cnt<< std::endl;
	}
	// Finalize: Any resources allocated for MPI can be freed
	MPI_Finalize();
	return 0;
}
