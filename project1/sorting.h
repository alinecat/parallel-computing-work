#include <iostream>
#include <algorithm>
#include <vector> 
#include "util.h"
#include <climits>

using namespace std;


//This function determines the comparison method in sort function
bool compare(keyPair a, keyPair b){
	if(a.secondKey < b.secondKey)
		return 1;
	else 
		return 0;
}



void bucket_sort(keyPair kpArray[], int kpSize)
{
		//Bucket Sort Alg. for Serial Implementation.
		//First, puts the key pairs into a vector. The algorithm normalizes the first key.		
		int min=INT_MAX, max=INT_MIN; 
		
		for(int i = 0; i < kpSize; i++) 
		{
			if (max < *((&kpArray->firstKey)+(2*i)))
				max = *((&kpArray->firstKey)+(2*i));
			if (min > *((&kpArray->firstKey)+(2*i)))
				min = *((&kpArray->firstKey)+(2*i));
		}		

    int range = max - min ;
    
    vector<keyPair> b[range+1];
    
    for (int i=0; i<kpSize; i++)
    {
       int x = kpArray[i].firstKey-min;
       b[x].push_back(kpArray[i]);
    }
    
    //Sorting the buckets and ordering the kpArray
		//Sort fnc. is std lib implementaion. It uses insertion sort (as far as I checked)
    for (int i=0; i<range+1; i++)
       sort(b[i].begin(), b[i].end(),compare);
    int index = 0;
    for (int i = 0; i < range+1; i++)
        for (int j = 0; j < b[i].size(); j++)
          kpArray[index++] = b[i][j];
}


void partial_bucket_sort(keyPair kpArray[], keyPair partial_kpArray[], int kpSize, int bucket_id_start, int bucket_id_end)
{
		
		//This is an implementaiton for the parallel code
		//This is a similar implementaion to the fnc. above, This one puts the result in a partial kpArray.
		int partial_kpArray_len = 0;
		int x = 0;
		int range = bucket_id_end-bucket_id_start;
		vector<keyPair> b[range+1];
		
		for(int i = 0; i < kpSize; i++) 
		{
			if  ((bucket_id_start <= kpArray[i].firstKey) && (kpArray[i].firstKey <= bucket_id_end))
			{ //Copy the elements to be sorted
				x = kpArray[i].firstKey-bucket_id_start;
				b[x].push_back(kpArray[i]);  
				partial_kpArray_len++;
			}
		}		
		
	
    for (int i=0; i<range+1; i++)
       sort(b[i].begin(), b[i].end(),compare);
    int index = 0;
    for (int i = 0; i < range+1; i++)
        for (int j = 0; j < b[i].size(); j++)
          partial_kpArray[index++] = b[i][j];
    

}
