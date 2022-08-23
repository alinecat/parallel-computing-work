#include <stdio.h>
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

	
	if (n%2 == 0)
		med_idx = n/2  ;
	else 
		med_idx = (n+1)/2 -1 ;
	comparison_obj comp_obj;
	std::vector<Point> P_vect(P,P+n);
	comp_obj.j = k ;
	std::sort(P_vect.begin(),P_vect.end(), comp_obj);
	M = P_vect[med_idx];

	return M;
}


Node *kdTreeConstruct(Point *P, int dimension, int k, int n)
{
	Point M;
	if (n <= 1)
	{	
		if (n == 1)
		{//This is denoting the leaf node, no more child nodes to construct
			Node *N = newNode(P[0]); 
			return N; 
		}
		else 
		{//Assign a NULL ptr to the node if the tree is traversed fully
			return NULL;
		}
	}
	//Find the median of the points in kth dimension
	M = findMedian(P, k, n) ;
	//N is the median node that will have two child node
	Node *N = newNode(M) ;
	
	int left_size, right_size;
	if (n%2 == 0)
	{
		left_size = n/2;
		right_size = n/2 -1;
	}
	else if (n%2 == 1)
	{
		left_size = n/2;
		right_size = n/2;
	}
	
	Point *Pleft = newPoints(left_size, dimension);
	Point *Pright = newPoints(right_size, dimension);
	
	
	int left_idx = 0 ;
	int right_idx = 0 ;
	
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
	N->left = kdTreeConstruct(Pleft,dimension,k,left_size);
	N->right = kdTreeConstruct(Pright,dimension,k,right_size);
	return N;
}





int rangeSearch(Node *node, int dimension, int k, Range range)
{
	
	if (node == NULL)
		return 0;
	
	int result_pts = 0;
	int result_pts_right = 0;
	int result_pts_left = 0;
	bool result_flag = true ;
	for (int i = 0; i < dimension ; i++)
	{
		if (!((node->data[i] >= range.leftPoint[i])&(node->data[i] <= range.rightPoint[i])))
		{
			//printf("node data:%d\n",range.leftPoint[i]);
			result_flag = false;
			break;
		}
	}
	if (result_flag)
		result_pts++;
	
	int k_next = (k+1)%dimension;

	if (node->data[k] >= range.leftPoint[k])
	{	
		result_pts += rangeSearch(node->left, dimension, k_next ,range);
	}
	if (node->data[k] <= range.rightPoint[k])
	{
		result_pts += rangeSearch(node->right, dimension, k_next ,range);
	}
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
  
  /*Commented, to prevent printing
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
  
   
  root = kdTreeConstruct(points , dimension, 0, numPoints);
 
  printf("Printing the tree..\n");
  //printTree(root, dimension);
  
  
  
  Result *results = new Result[numQueries];
  
  /////////////////////////////////////////////////
  ///////         Search Query Ranges  ////////////
  /////////////////////////////////////////////////
  
  
  for (int i = 0; i < numQueries; i++)
  {
    results[i].size = rangeSearch(root, dimension, 0, queries[i]); 
  }  
  
  /*
  printf("Results:\n");

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


