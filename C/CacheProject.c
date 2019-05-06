#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

int main(int argc, char *argv[]){
	long long unsigned int readnumhits = 0, readnum_misses = 0, writenumhits =0, writenum_misses = 0, address = 0;
	int cachesize = atoi(argv[1]);    // convert cachesize to int
	int associativity = atoi(argv[2]); // convert associativity to int
	int replacement= atoi(argv[3]); //get the replacement policy of the cache
	int write = atoi(argv[4]); // 0 == write through, 1 == Write back 
	int blocksize = 64; //fixed block size of 64
	FILE *filepassed = fopen(argv[5], "r"); // open file for reading only
	
	//Find the number of sets in the cache
	int sets = cachesize / (blocksize*associativity);

	//Need a 2d array with size big enough for 64bit address
	long long unsigned int **cache;
	cache = malloc(sets*sizeof(long long unsigned int*));
	
	//alllocate memory for each of the 64 bit addresses 
	for(int i=0;i<sets;i++)
	{
		cache[i] = malloc(associativity * sizeof(long long unsigned int));
		memset(cache[i],1000000, associativity * sizeof(long long unsigned int));
		if(cache[i] == NULL){
			fprintf(stderr, "out of memory/n");
			exit(1);
		}
	}

	int **dirty;
	dirty = malloc( sets * sizeof(int*) );
	for(int i=0 ; i < sets ; i++)
	{
		dirty[i] = malloc(associativity * sizeof(int));
		memset(dirty[i], 0, associativity * sizeof(int));
		if(dirty[i] == NULL){
			fprintf(stderr, "out of memory/n");
			exit(1);
		}
	} 	

	//create the buffer to read in from the file
	char buffer[30];
	char RW;
	unsigned int mem_writes = 0;
	unsigned int mem_reads = 0;
	
	// Read the file line by line
	while(fgets(buffer, 30, filepassed))
	{
		//I know what the data looks like so i can use sscanf()
		RW = 'R';
		while(sscanf(buffer, "%c %llx/n", &RW, &address) != 2){
			if(fgets(buffer, 30, filepassed)==NULL)
				break;
		}
		// Find the tag bits by removing the offset and the index bits
		long long unsigned int comparetag = address / blocksize / sets; 
		long long unsigned int index = (address / blocksize) % sets; //get the correct index to access
		int j = 0;
		int hitflag = 0;
		
		for( j = 0; j < associativity; j++ ){

			//check for a read hit
			if ( cache[index][j] == comparetag )
			{	
				hitflag = 1;
				if( RW == 'R' )
				{
					readnumhits++; //we have a read hit
					if(replacement == 1)
					{
						break;
					}

					int z;
					long long unsigned int temp =  cache[index][j];
					int temp2 = dirty[index][j];
					//update set by shifting to the right
					for( z = j ; z > 0 ; z--)
					{
						//shift every element to the right by 1 value
						cache[index][z] = cache[index][z-1];
						dirty[index][z] = dirty[index][z-1];
					}
					cache[index][0] = temp; //update the MRU position
					dirty[index][0] = temp2;

				}
				else 
				{
					writenumhits++; //we have a write hit
					if ( write == 0 )
					{
						mem_writes++;
					}

					if(replacement == 1)
					{
						break;
					}
					int z;
					long long unsigned int temp =  cache[index][j];
					int temp2 = dirty[index][j];
					//update set by shifting to the right
					for(z=j;z>0;z--){
						//shift every element to the right by 1 value
						cache[index][z] = cache[index][z-1];
						dirty[index][z] = dirty[index][z-1];
					}
					//update the MRU position
					cache[index][0] = temp;
					dirty[index][0] = temp2;
				}
				break;
			}
		}
 
		if( hitflag == 0 )
		{
			/* at this pointwe have iterated through entire set
				and no hit has occured then we know we have a miss
				*/
			if( RW == 'R' )
			{
				readnum_misses++;
				mem_reads++;
			}
			if( RW == 'W' )
			{
				writenum_misses++;
				if ( write == 0 )
				{
					mem_writes++;
					mem_reads++;
				}
				else 
				{
					mem_reads++;
				}
			}

			if ( write == 1 )
			{
				if ( dirty[index][associativity - 1] == 1 )
				{
					mem_writes++;
				}
			}

			//Updating the set by shifting out the rightmost entry and all others to the right by 1
			int a = 0;
			long long unsigned int replaced = cache[index][associativity - 1];
			for( a = associativity - 1 ; a > 0 ; a--)
			{
				cache[index][a] = cache[index][a-1];
				dirty[index][a] = dirty[index][a-1];
			} 
			cache[index][0] = comparetag;

			if (replaced != 1000000)
			{
				dirty[index][0] = 1;
			}
		}
	}//end of whileloop to parse the string
		
		printf("Number of mem reads: %lld\n", mem_reads);
		printf("Number of mem writes: %lu\n", mem_writes);
		printf("Miss ratio: %lf \n", ((writenum_misses+readnum_misses)*1.0/(readnumhits+readnum_misses+writenum_misses+writenumhits)));
		free( cache );
		free( dirty );
	} //end of main
