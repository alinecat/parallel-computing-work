/*
 * util.h
 *
 *  Created on: Oct 18, 2021
 *      Author: ali
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>

//Representation of a single point in d-dimension
typedef int PointTerm;
typedef int *Point;

struct Range; /* Forward declaration */
typedef struct Range
{
    Point leftPoint;
    Point rightPoint;
} Range;

struct Result; /* Forward declaration */
typedef struct Result
{
    Point *resultPoints;
    int size;
} Result;

int read_points_file(char *file_name, int *numPoints, int *dimension, Point **points);

int read_queries_file(char *file_name, int *numQueries, int *dimension, Range **queries);

void write_results(char *fileName, Result *results, int numQueries, int dimension);

Range *newQueries(int numQueries, int dimension);

void printPoint(Point point, int dimension);

void fprintPoint(FILE *__restrict__ inputFile, Point point, int dimension);

Point *newPoints(int numPoints, int dimension);

// Tree representation taken from https://www.geeksforgeeks.org/binary-tree-set-1-introduction/
struct Node; /* Forward declaration */
typedef struct Node
{
    Point data;
    Node *left;
    Node *right;
} Node;

Node *newNode(Point data);

void printTree(Node *root, int dimension);

#endif /* UTIL_H_ */
