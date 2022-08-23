#include <fstream>
#include <iostream>
#include <vector>

#include "util.h"

void read_file_part_a(const char *file_name, int **array, int *size){
   std::ifstream infile(file_name);
   //declaring vector of pairs
   std::vector<int> vect;
   
   int a;
    while (infile >> a)
    {
        vect.push_back(a);
    }
    *array = (int*) malloc(vect.size() * sizeof(int));
    for(int i=0; i < vect.size(); i++){
        (*array)[i] = vect[i];
    }
    *size = vect.size();
}


void read_file_part_b(const char *file_name, keyPair **kpArray, int *size){
   std::ifstream infile(file_name);
   //declaring vector of pairs
   std::vector< std::pair <int,int> > vect;
   
   int a, b;
    while (infile >> a >> b)
    {
        vect.push_back( std::make_pair(a,b) );
    }
    *kpArray = (keyPair*) malloc(vect.size() * sizeof(keyPair));
    for(int i=0; i < vect.size(); i++){
        (*kpArray)[i].firstKey = vect[i].first;
        (*kpArray)[i].secondKey = vect[i].second;
    }
    *size = vect.size();
}
