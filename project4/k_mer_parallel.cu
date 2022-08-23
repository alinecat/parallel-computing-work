#include <util.h>
#include <vector>
#include <stdio.h>
#include <cuda.h>
#define MAX_KMER 14
#define MIN_KMER 2





#define READ_LEN_MAX 200
#define REF_LEN_MAX 1024*1024
#define READ_CNT_MAX 1024*20


int num_kmer_in_read(char *read, int k);


static void HandleError( cudaError_t err,
                         const char *file,
                         int line ) {
    if (err != cudaSuccess) {
        printf( "%s in %s at line %d\n", cudaGetErrorString( err ),
                file, line );
        exit( EXIT_FAILURE );
    }
}
#define HANDLE_ERROR( err ) (HandleError( err, __FILE__, __LINE__ ))


__device__ int d_strlen(const char* string){
    int length = 0;
    while (*string++)
        length++;
    return (length);
}

//Compares string until nth character, m times
//Modify with m to enable load balancing between threads
__device__ int d_strncmp( const char * s1, const char * s2, size_t n, int m )
{
    while ( n && *s1 && ( *s1 == *s2 ) && --m )
    {
        ++s1;
        ++s2;
        --n;
    }
    if ( (n == 0) && (m == 0) )
    {
        return 0;
    }
    else
    {
        return ( *(unsigned char *)s1 - *(unsigned char *)s2 );
    }
}


__global__ void match_k_mer_g(int res_arr[], int res_cnt[], int k_mer, char* read_str, char* reference_str , int read_len, int ref_len, int read_cnt)
{

	//Gets each k-mer, assigns it to a thread
	int offset2 = blockIdx.x + read_len*blockIdx.y ;
	int result;

	//Go over the ref_len, find the matching sequences
	for(int i = 0; i < ref_len; i++)
	{
		//Modified version of d_strncmp is used. It checks the first k_mer elements of the two strings
		result = d_strncmp((reference_str+i),(read_str+offset2) , k_mer, k_mer);
		
		//If there is a match, store the index i and increase the count
		if (result == 0) 
		{

			//if the index is empty or there exists a smaller index:
			if ((res_arr[offset2] == -1)||( i < res_arr[offset2] ))
			{
				//write the first matched index or smallest index, for every other match increase the count (else)
				res_arr[offset2] = i;
				res_cnt[offset2]++;
			}
			else
			{
				//printf("HERE");
				res_cnt[offset2]++;
			}
	}
	
	}

}


__host__ void match_k_mer_h(int res_arr[], int cnt[], char* ref_str, char* read_str, int k_mer, int read_len, int ref_len, int read_cnt)
{
	int* dev_result_idx_arr ;
	int* dev_res_cnt_arr ;
	char* dev_ref_str;
	char* dev_read_str;
	
	int size_ref = REF_LEN_MAX*sizeof(char);
	int size_rds = READ_LEN_MAX*READ_CNT_MAX*sizeof(char);
	int size_res = READ_CNT_MAX*(READ_LEN_MAX-MIN_KMER+1)*sizeof(int);
	int size_res_cnt = size_res;
	


	HANDLE_ERROR( cudaMalloc((void**)&dev_ref_str,size_ref) );
	HANDLE_ERROR( cudaMalloc((void**)&dev_read_str,size_rds) );
	HANDLE_ERROR( cudaMalloc((void**)&dev_result_idx_arr,size_res) );
	HANDLE_ERROR( cudaMalloc((void**)&dev_res_cnt_arr,size_res_cnt) );
	
	//Copy the result arrays, host set them to -1 (result_arr) and 0 (cnt_arr)
	HANDLE_ERROR( cudaMemcpy( dev_ref_str, ref_str, size_ref, cudaMemcpyHostToDevice) );
	HANDLE_ERROR( cudaMemcpy( dev_read_str, read_str, size_rds, cudaMemcpyHostToDevice) );
	HANDLE_ERROR( cudaMemcpy( dev_result_idx_arr, res_arr, size_res, cudaMemcpyHostToDevice) );
	HANDLE_ERROR( cudaMemcpy( dev_res_cnt_arr, cnt,size_res_cnt, cudaMemcpyHostToDevice) );
	
	
	dim3 dimBlock(read_len-k_mer+1,read_cnt,1);
	dim3 dimThread(1,1);
	

	
	match_k_mer_g<<<dimBlock,dimThread>>> (dev_result_idx_arr, dev_res_cnt_arr, k_mer, dev_read_str, dev_ref_str, read_len, ref_len, read_cnt);
	
	
	
	HANDLE_ERROR( cudaMemcpy(res_arr, dev_result_idx_arr, size_res, cudaMemcpyDeviceToHost) );
	HANDLE_ERROR( cudaMemcpy(cnt, dev_res_cnt_arr, size_res_cnt, cudaMemcpyDeviceToHost) );
	
	

	HANDLE_ERROR( cudaFree( dev_result_idx_arr ) );
	HANDLE_ERROR( cudaFree( dev_res_cnt_arr ) );
    HANDLE_ERROR( cudaFree( dev_ref_str ) );
    HANDLE_ERROR( cudaFree( dev_read_str ) );
}





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
    /*Comment the print
    if(success){
        for(int i = 0; i < queries.used; i++) {
            //printf("read : %s\n", queries.array[i]);
        }
    }
    */
    
   
    
    ////////////////////////////////////////////////////////////////////////
    ////////////// THIS IS A GOOD PLACE TO DO YOUR COMPUTATIONS ////////////
    ////////////////////////////////////////////////////////////////////////
    


	//Flatten the queries
	const int read_length = strlen(queries.array[0]);
	char* flat_read_str = (char*) malloc( read_length*(queries.used)*sizeof(char));
	
    for(int i = 0; i < queries.used; i++) 
    {
    	for(int j = 0; j < read_length; j++)
    	{
    		flat_read_str[i*read_length+j] = queries.array[i][j];
    	}
    }



	int* result_idx_arr ;
	int* cnt ;
	
	int len_res_arr = READ_CNT_MAX*(READ_LEN_MAX-MIN_KMER+1);
	result_idx_arr = (int*)malloc(len_res_arr*sizeof(int));
	cnt = (int*)malloc(len_res_arr*sizeof(int));
	
	//Set the result array to -1 and cnt array to 0
 	for(int i=0; i < len_res_arr ; i++)
 	{
 		result_idx_arr[i] = -1;
 		cnt[i] = 0;
 	}
	
	
	//Call Host Function
	match_k_mer_h(result_idx_arr, cnt ,reference_str, flat_read_str, k, read_length, reference_length, queries.used);

	
	
	int k_mer_cnt_arr[queries.used];
	//Reduce the count values for read sequences
    for(int i=0; i < queries.used ; i++)
 	{
 		k_mer_cnt_arr[i] = 0;
 		for(int j = 0; j < read_length-k+1; j++)
 		{
 			k_mer_cnt_arr[i] = k_mer_cnt_arr[i] +cnt[i*(read_length)+j];
 		}
 	}    
 	
 	
 	//Print the output to the file
 	FILE *fp_out;
    fp_out = fopen(output_filename, "w");
    for(int i=0; i < queries.used ; i++)
 	{
 		fprintf(fp_out,"%d",k_mer_cnt_arr[i]) ;
 		for(int j = 0; j < read_length-k+1; j++)
 		{
 			fprintf(fp_out," %d",result_idx_arr[i*(read_length)+j]) ;
 		}
 		fprintf(fp_out,"\n") ;
 		//printf("%d\n",result_idx_arr[i]) ;
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
    
    return 0;
}


