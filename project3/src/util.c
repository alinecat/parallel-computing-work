/*
 * util.cpp
 *
 *  Created on: Oct 18, 2021
 *      Author: ali
 */

#include "util.h"

#include <stdlib.h>

int read_points_file(char *file_name, int *numPoints, int *dimension, Point **points)
{
	printf("Reading the file...\n");
	FILE *inputFile = fopen(file_name, "r");
	if (inputFile)
	{
		int success;

		success = fscanf(inputFile, "%d", &*numPoints);
		if (!success)
		{
			printf("Bad File format!\n");
			return 0;
		}
		success = fscanf(inputFile, "%d", &*dimension);
		if (!success)
		{
			printf("Bad File format!\n");
			return 0;
		}

		*points = newPoints(*numPoints, *dimension);

		for (int i = 0; i < *numPoints; i++)
		{
			for (int j = 0; j < *dimension; j++)
			{
				success = fscanf(inputFile, "%d", &((*points)[i][j]));
				if (success == EOF || success == 0)
				{
					printf("Bad File format!\n");
					return 0;
				}
			}
		}
		// for (size_t i = 0; i < *numPoints; i++)
		// {
		// 	printPoint((*points)[i], *dimension);
		// 	printf("\n");
		// }

		fclose(inputFile);
		return 1;
	}
	printf("Could not open the file!\n");
	return 0;
}

int read_queries_file(char *file_name, int *numQueries, int *dimension, Range **queries)
{
	printf("Reading the file...\n");
	FILE *inputFile = fopen(file_name, "r");
	if (inputFile)
	{
		int success;

		success = fscanf(inputFile, "%d", &*numQueries);
		if (!success)
		{
			printf("Bad File format!\n");
			return 0;
		}
		success = fscanf(inputFile, "%d", &*dimension);
		if (!success)
		{
			printf("Bad File format!\n");
			return 0;
		}

		*queries = newQueries(*numQueries, *dimension);

		for (int i = 0; i < *numQueries; i++)
		{
			for (int j = 0; j < *dimension; j++)
			{
				success = fscanf(inputFile, "%d", &((*queries)[i].leftPoint[j]));
				if (success == EOF || success == 0)
				{
					printf("Bad File format!\n");
					return 0;
				}
			}
			for (int j = 0; j < *dimension; j++)
			{
				success = fscanf(inputFile, "%d", &((*queries)[i].rightPoint[j]));
				if (success == EOF || success == 0)
				{
					printf("Bad File format!\n");
					return 0;
				}
			}
		}
		fclose(inputFile);
		return 1;
	}
	printf("Could not open the file!\n");
	return 0;
}

void write_results(char *fileName, Result *results, int numQueries, int dimension)
{
	FILE *inputFile = fopen(fileName, "w");
	if (inputFile)
	{
		//Output the final values
		for (int i = 0; i < numQueries; i++)
		{
			// fprintf(inputFile,"%d:", results[i].size);
			// for (int j = 0; j < results[i].size; j++)
			// {
			// 	fprintPoint(inputFile,results[i].resultPoints[j], dimension);
			// 	fprintf(inputFile," ");
			// }
			// fprintf(inputFile,"\n");

			//Printing only the results size
			fprintf(inputFile,"%d\n", results[i].size);
		}
	}
	else
	{
		printf("Could not open the file!\n");
	}
}

Range *newQueries(int numQueries, int dimension)
{
	Range *ranges;

	ranges = (Range *)malloc(sizeof(Range) * numQueries + sizeof(PointTerm) * dimension * numQueries * 2);
	Point ptr;
	// ptr is now pointing to the first element in of 2D array
	ptr = (Point)(ranges + numQueries);

	// for loop to point rows pointer to appropriate location in 2D array
	for (int i = 0; i < numQueries; i++)
	{
		ranges[i].leftPoint = (ptr + 2 * dimension * i);
		ranges[i].rightPoint = (ptr + 2 * dimension * i + dimension);
	}

	return ranges;
}

void printPoint(Point point, int dimension)
{
	printf("(");
	for (size_t i = 0; i < dimension - 1; i++)
	{
		printf("%d,", point[i]);
	}
	printf("%d)", point[dimension - 1]);
}

void fprintPoint(FILE *__restrict__ inputFile, Point point, int dimension)
{
	fprintf(inputFile, "(");
	for (size_t i = 0; i < dimension - 1; i++)
	{
		fprintf(inputFile, "%d,", point[i]);
	}
	fprintf(inputFile, "%d)", point[dimension - 1]);
}

Point *newPoints(int numPoints, int dimension)
{
	Point *points;

	points = (Point *)malloc(sizeof(Point) * numPoints + sizeof(PointTerm) * dimension * numPoints);
	Point ptr;
	// ptr is now pointing to the first element in of 2D array
	ptr = (Point)(points + numPoints);

	// for loop to point rows pointer to appropriate location in 2D array
	for (int i = 0; i < numPoints; i++)
	{
		points[i] = (ptr + dimension * i);
	}
	return points;
}

/* newNode() allocates a new node
with the given data and NULL left
and right pointers. */
Node *newNode(Point data)
{
	// Allocate memory for new node
	Node *node = (Node *)malloc(sizeof(Node));

	// Assign data to this node
	node->data = data;

	// Initialize left and
	// right children as NULL
	node->left = NULL;
	node->right = NULL;
	return (node);
}

void _printTree(Node *root, int dimension, int *_level)
{
	printf("|");
	for (int ix = 0; ix < *_level; ++ix)
	{
		printf("----");
	}
	printPoint(root->data, dimension);
	printf("\n");
	*_level = *_level + 1;
	if (root->left != NULL)
	{
		_printTree(root->left, dimension, _level);
		*_level = *_level - 1;
	}
	if (root->right != NULL)
	{
		_printTree(root->right, dimension, _level);
		*_level = *_level - 1;
	}
}

void printTree(Node *root, int dimension)
{
	int level = 0;
	_printTree(root, dimension, &level);
}
