#include <util.h>
#include <vector>
#define MAX_KMER = 15
#define MIN_KMER = 1


int num_kmer_in_read(char *read, int k);
void hash(char *seq,int seq_len, int k_mer_len, int hashed_seq[]);

int main(int argc, char** argv)
{
    if(argc != 5) {
        printf("Wrong argments usage: ./kmer [REFERENCE_FILE] [READ_FILE] [k] [OUTPUT_FILE]\n" );
    }

    FILE *fp;
    int k;

    //malloc instead of allocating in stack
    char *reference_str = (char*) malloc(MAX_REF_LENGTH * sizeof(char));
    char *read_str = (char*) malloc(MAX_READ_LENGTH * sizeof(char));

    char *reference_filename, *read_filename, *output_filename;
    int reference_length;

    reference_filename = argv[1];
    read_filename = argv[2];
    k = atoi(argv[3]);
    output_filename = argv[4];

    fp = fopen(reference_filename, "r");
    if (fp == NULL) {
        printf("Could not open file %s!\n",reference_filename);
        return 1;
    }

    if (fgets(reference_str, MAX_REF_LENGTH, fp) == NULL) { //A single line only
        printf("Problem in file format!\n");
        return 1;
    }
    reference_str[strcspn(reference_str, "\n")] = 0; //Remove the trailing \n character
    
    reference_length = strlen(reference_str);

	
    //printf("Reference str is = %s\n", reference_str);
    fclose(fp);

    //Read queries
    StringList queries;

    initStringList(&queries, 3);  // initially 3 elements

	
    int success = read_file(read_filename,&queries);
    /* COMMENTED THE PRINTING
    if(success){
        for(int i = 0; i < queries.used; i++) {
            printf("read : %s\n", queries.array[i]);
        }
    }
    */
    
    ////////////////////////////////////////////////////////////////////////
    ////////////// THIS IS A GOOD PLACE TO DO YOUR COMPUTATIONS ////////////
    ////////////////////////////////////////////////////////////////////////
    
    
    //Go over the hashed read_strings
    
    //HASH THE REFERENCE STRING
	int ref_hash_arr[reference_length-k+1]; 

	hash(reference_str,reference_length, k, ref_hash_arr); 
	
   
    //HASH THE READ STRINGS
    
   
	const int read_length = strlen(queries.array[0]);
	int read_hash_arr[queries.used][read_length-k+1]; 
	
	
	
	int* result_idx_arr ;
	result_idx_arr = (int*)malloc((queries.used)*(read_length-k+1)*(read_length-k+1)*sizeof(int));
	
	//Create an array for the counts in a read sequence
	int k_mer_cnt_arr[queries.used];
	int k_mer_cnt = 0;
	
	
	for(int i = 0; i < queries.used; i++) 
    {
    	hash(queries.array[i],read_length, k, read_hash_arr[i]); 
    }
    
	
	  
	
    std::vector<int> idx_arr ;
    //Store the indexes in a dynamic data structure
    
    //Go over the number of reads and the k-mers in a read "reference_length-k+1" points to the k-mer
	for(int a = 0; a < queries.used; a++) 
    {
    	for(int b = 0; b< read_length-k+1; b++)
   		{
   			
   			for(int c = 0; c < reference_length-k+1; c++)
    		{
    			if(ref_hash_arr[c] == read_hash_arr[a][b])
    			{
    				//If there is a match, store the index, increase the count
    				idx_arr.push_back(c);
    				k_mer_cnt++;
    			}
    		} 
    		if(idx_arr.size() == 0)
    		{ 
    			//If there is no match, store -1 as index
    			idx_arr.push_back(-1);

    		}
    		
    		//Copy the results to the final result array
    		for(int m=0; m < idx_arr.size(); m++ ) 
    		{
        		result_idx_arr[a*(read_length-k+1)*(read_length-k+1)+b*(read_length-k+1)+m] = idx_arr[m];
    		}
    		idx_arr.clear();
			
    	}
    	//Copy the count to the corresponding part of count array
    	k_mer_cnt_arr[a] = k_mer_cnt;
    	k_mer_cnt = 0;
 	}
    
    
    
    // Print the outputs to the file
    FILE *fp_out;
    fp_out = fopen(output_filename, "w");
    
	for(int a = 0; a < queries.used; a++) 
    {
    	fprintf(fp_out,"%d", k_mer_cnt_arr[a]);
    	for(int b = 0; b< read_length-k+1; b++)
   		{ 
			fprintf(fp_out," %d",result_idx_arr[a*(read_length-k+1)*(read_length-k+1)+b*(read_length-k+1)+0]);
			
    	}
    	fprintf(fp_out, "\n");
    }    
    
	fclose(fp_out);
    
    
    
    ////////////////////////////////////////////////////////////////////////
    ////////////// THIS IS A GOOD PLACE TO DO YOUR COMPUTATIONS ////////////
    ////////////////////////////////////////////////////////////////////////
    
    
    
    //Free up
    freeStringList(&queries);
    free(reference_str);
    free(read_str);
    free(result_idx_arr);
}

void hash(char *seq,int seq_len, int k_mer_len, int hashed_seq[])
{

	//Check maximum k_mer length 
	if((k_mer_len > 14) || (k_mer_len < 2))
	{
		printf("Check k-mer length, not a valid value!");
	}
	
	
	//Set A to b_00
	//Set C to b_01
	//Set G to b_10
	//Set T to b_11
	
	/*
	Maximum k_mer length is 14. 
	This corresponds to 2*14, we can use a 32 bit integer for hash
	Shift the encoded letters in k-mer sequence
	*/
	
	//
	
	int hash = 0;
	
	for(int i = 0; i < seq_len-k_mer_len+1 ; i++)
	{
		for(int j = 0; j < k_mer_len ; j++)
		{
			if(seq[i+j] == 'A')
			{
				//OR with the encoding, 00 for A
				hash = hash|0;
				//Shift by 2 bit
				hash = hash << 2;
			}
			else if(seq[i+j] == 'C')
			{
				hash = hash|1;
				hash = hash << 2;
			}
			else if(seq[i+j]== 'G')
			{
				hash = hash|2;
				hash = hash << 2;
			}
			else if(seq[i+j] == 'T')
			{
				hash = hash|3;
				hash = hash << 2;
			}
		
		}
		//An extra shift is made, revert it.
		hash = hash >> 2;		
		hashed_seq[i] = hash;
		//Set hash to 0 for next k-mer
		hash = 0;
    }
}
