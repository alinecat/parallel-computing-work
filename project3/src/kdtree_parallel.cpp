#include <stdio.h>
#include <omp.h>
#include <algorithm>
#include <vector>
#include "util.h"
#include <chrono>
#include <iostream>



struct comparison_obj {
	int j;
	bool operator () (Point a, Point b) {return (a[j] < b[j]);}
};

Point findMedian(Point *P, int k, int n) //Search for the median point in the list P
{
	Point M;
	int med_idx;
	//there are two cases for median index, even or odd
	if (n%2 == 0)
		med_idx = n/2  ;
	else 
		med_idx = (n+1)/2 -1 ;
	
	//Create a comprison object that will sort the list by kth dimension element
	comparison_obj comp_obj;
	//Copy to a vector for sorting
	std::vector<Point> P_vect (P, P+n);
	comp_obj.j = k ;
	std::sort(P_vect.begin(),P_vect.end(), comp_obj);
	M = P_vect[med_idx];
	return M;
}


Node *kdTreeConstruct(Point *P, int dimension, int k, int n)
{
	Point M;
	if (n <= 1)
	{	if (n == 1)
		{//This is denoting the leaf node, no more child nodes to construct
			Node *N = newNode(P[0]); 
			return N; 
		}
		else 
		{//Assign a NULL ptr to the node if the tree is traversed fully
			Node *N = NULL; 
			return N;
		}
	}
	//Find the median of the points in kth dimension
	M = findMedian(P, k, n) ;
	//N is the median node that will have two child node
	Node *N = newNode(M) ;
	
	//Determine the sizes of right and left node
	int left_size, right_size;
	if (n%2 == 0)
	{
		left_size = n/2;
		right_size = n/2 -1;
	}
	else
	{
		left_size = n/2;
		right_size = n/2;
	}
	
	//Create the points for right and left of median node n kth dimension
	Point *Pleft = newPoints(left_size, dimension);;
	Point *Pright = newPoints(right_size, dimension);	;
	
	
	int left_idx = 0 ;
	int right_idx = 0 ;
	
	//Check the kth dimension of points and assign them to right or left point list
	for(int i = 0; i<n ;i++)
	{
		if (M[k] >= P[i][k])
		{
			if (P[i] != M){
				Pleft[left_idx] = P[i];
				left_idx += 1;
			}
		}
		if (M[k] <= P[i][k])
		{
			if (P[i] != M){
				Pright[right_idx] = P[i];
				right_idx += 1;
			}
		}
	}
	
	
	k += 1 ;
	k = k%dimension ;
	#pragma omp task shared(N) //Create recurisve tasks by using this directive
	N->left = kdTreeConstruct(Pleft,dimension,k,left_size);
	
	#pragma omp task shared(N) //Create recurisve tasks by using this directive 
	N->right = kdTreeConstruct(Pright,dimension,k,right_size);
	#pragma omp taskwait //Wait for a node to complete, both left and right
	return N;
}




int rangeSearch(Node *node, int dimension, int k, Range range)
{
	//Base case of recurison, if a NULL ptr is encountered end the task
	if (node == NULL)
		return 0;
	
    //int tid = omp_get_thread_num();
    //printf("Recurison from omp thread %d\n", tid);
	int result_pts = 0;
	int result_pts_right = 0;
	int result_pts_left = 0;
	
	//This flag is used to check whether the every entry of a point is in the range or not
	bool result_flag = true ;
	for (int i = 0; i < dimension ; i++)
	{
		if (!((node->data[i] >= range.leftPoint[i])&(node->data[i] <= range.rightPoint[i])))
		{//If every entry of a point is not in the range, this point is not in the range
			result_flag = false;
			break;
		}
	}
	if (result_flag)
		result_pts++;
	
	int k_next = (k+1)%dimension;
	
	//Create parallel tasks, each thread should be responsible from a new recurison
	
	
	if (node->data[k] >= range.leftPoint[k])
	{	
		//#pragma omp task shared(result_pts_left)
		result_pts_left = rangeSearch(node->left, dimension, k_next ,range);
	}

	//Create parallel tasks, each thread should be responsible from a new recurison
	

	if (node->data[k] <= range.rightPoint[k])
	{	
		//#pragma omp task shared(result_pts_right)
		result_pts_right = rangeSearch(node->right, dimension, k_next ,range);
	}
	//Combine the results from left, right and current node tasks into the result points
	//#pragma omp taskwait
	result_pts = result_pts_right + result_pts_left + result_pts;
	return result_pts;

}
 


int main(int argc, char **argv)
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	if (argc != 4)
	{
	printf("Wrong argments usage: kdtree_serial [INPUT_POINTS_FILE] [INPUT_QUERIES_FILE] [OUTPUT_FILE]\n");
	return -1;
	}
	
	int dimension, numPoints, numQueries, dimensionQ;
	Range *queries;
	Point *points;
	
	int success = read_points_file(argv[1], &numPoints, &dimension, &points);
	if (success)
	{
	printf("Successful, number of points are %d, dimension is %d\n", numPoints, dimension);
	}
	else
	{
	printf("Unsuccessful file read operation for points file, exiting the program!\n");
	return -1;
	}
	success = read_queries_file(argv[2], &numQueries, &dimensionQ, &queries);
	if (success)
	{
	printf("Successful, number of queries are  are %d\n", numQueries);
	}
	else
	{
	printf("Unsuccessful file read operation for queries file, exiting the program!\n");
	return -1;
	}
	if (dimension != dimensionQ)
	{
	printf("Points dimensions and query points dimensions don't match!\n");
	return -1;
	}
	
	
	//Commented this section because it becomes too crowded in the terminal
	/*
	printf("Points are:\n");
	for (size_t i = 0; i < numPoints; i++)
	{
	 printPoint(points[i], dimension);
	 printf("\n");
	}
	printf("Queries are:\n");
	for (int i = 0; i < numQueries; i++)
	{
	 printf("Query %d, from ", i);
	 printPoint(queries[i].leftPoint, dimension);
	 printf(" to ");
	 printPoint(queries[i].rightPoint, dimension);
	 printf("\n");
	}
	*/
	Node *root;
  
  
  
  
  
  
  
  
  
	/////////////////////////////////////////////////
	///////         CREATE K-d Tree      ////////////
	/////////////////////////////////////////////////


  
	
	omp_set_num_threads(16);
	omp_set_dynamic(0);
	//Create a recursive task that spans as a tree
	#pragma omp parallel shared(root)
	{
		#pragma omp single
		{
			root = kdTreeConstruct(points , dimension, 0, numPoints);
		}
	}
	
	
	printf("Printing the tree..\n");
	
	//printTree(root, dimension);
	
	
	/////////////////////////////////////////////////
	///////         Search Query Ranges  ////////////
	/////////////////////////////////////////////////
	
	
	Result *results = new Result[numQueries]; 
	#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < numQueries; i++)
	{	    
		results[i].size = rangeSearch(root, dimension, 0, queries[i]);
	} 
	
	
	//Commented this section because it becomes too crowded in the terminal
	/*
	for (int i = 0; i < numQueries; i++)
	{
		printf("%d:",results[i].size);
		for (int j = 0; j < results[i].size; j++)
		{
			//printPoint(results[i].resultPoints[j],dimension); //YOU DON'T NEED TO COMPUTE THE POINTS
			printf(" ");
		}
		printf("\n");
	}
	*/
	
	
	write_results(argv[3],results,numQueries,dimension);
	
	
	delete points; //points should be deleted after we process the queries, kd-tree nodes points to elements in the points array
	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Elapsed time is:" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()/1000000.0 << "s" << std::endl;
	return 0;
}
