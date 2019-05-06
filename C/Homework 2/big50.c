//Nicholas Poidomani 
//NI915898
//COP3502C 


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "big50.h"

//adds the 50 digit integer 'p' to 'q' and returns the sum 'x'
Integer50 *big50Add(Integer50 *p, Integer50 *q){
		
		//sanity checks on 'p' and 'q' == NULL, returns NULL if true
		if ( p == NULL || q == NULL ){
		return NULL;
		}
		
	//malloc for the size of the struct 	
	Integer50 *x = malloc(sizeof(Integer50));
		if ( x == NULL ){
			return NULL;
		}
	//set all values LOW
	memset(x->digits, 0, MAX50*sizeof(int));
	
	//add the digits of 'p' to each digit of 'q'
	int i;
	for( i = 0 ; i<=MAX50-1 ; i++) {
		x->digits[i] = p->digits[i] + q->digits[i];
	}
	
	//now check for overflow in each digit
	for( i = 0 ; i<= MAX50 - 1 ; i++ ){
		if (x->digits[i]>=10 && i < MAX50  ){
			int r = x->digits[i] % 10;
			x->digits[i] = r;
			x->digits[i+1] = x->digits[i+1] + 1;
		}
	
	}
	big50Destroyer(p);
	big50Destroyer(q);
	return x;
}

//function to free the space that we allocated for our big number
Integer50 *big50Destroyer(Integer50 *p){
	if (p == NULL){
		return NULL;
	}
	else {
		free(p);
		return NULL;
	}
}

//Function takes a string and returns an integer with 50 digits
Integer50 *parseString(char *str){
	
	//Check if the argument 'str' is a valid string
	if ( str == "" ) {
		Integer50 *x = malloc(sizeof(Integer50));
		if ( x == NULL ){
			return NULL;
		}
		memset(x->digits, 0, MAX50*sizeof(int));
		return x;
	}
	else if ( str == NULL ) {
		return NULL;
	}
	
	//Allocate memory to the srtuct
	Integer50 *x = malloc(sizeof(Integer50));
		if ( x == NULL ){
			return NULL;
		}
	
	//Parse the string based on its length
	int len = strlen(str);
		if (len > MAX50 ){
			fprintf(stderr, "This string is not 50 digits long!\n"); //Requirement to write to stderr on special case

			int i;
			char p[2] = " ";
			x->digits = malloc(MAX50*sizeof(int));
				if (x->digits == NULL ) {
				free(x);
				return NULL;
				}
			memset(x->digits, 0, MAX50*sizeof(int));
			
			//string input is formatted such that the last char is the least significant digit
			int j = 0;
			for(i = len-1; i >= i - MAX50; i--){
			p[1] = str[i];
			x->digits[j] = atoi(p);
			j++;
			}
		return x;

		}
		else if ( len < MAX50 ){
			fprintf(stderr, "This string is not 50 digits long!\n");
			char p[2] = " ";
			x->digits = malloc(MAX50*sizeof(int));
				if (x->digits == NULL ) {
					free(x);
					return NULL;
				}
			memset(x->digits, 0, MAX50*sizeof(int));
			int i;
			int j = 0;
			for(i = len - 1 ; i>= 0 ; i--){
				p[1] = str[j];
				x->digits[i] = atoi(p);
				j++;	
			}
		
		}	
	return x;

		
}

//Function that does the fibonacci sequence on numbers with 50 digits
Integer50 *fibBig50(int n, Integer50 *first, Integer50 *second){
    Integer50 *sum;
    Integer50 *temp1 = malloc(sizeof(Integer50));
    	if ( temp1 == NULL ){
			return NULL;
		}
    temp1->digits = malloc(sizeof(int)*MAX50);
    	if ( temp1->digits == NULL ){
    		free(temp1);
		return NULL;
		}
    Integer50 *temp2 = malloc(sizeof(Integer50));
    	if ( temp2 == NULL ){
		return NULL;
		}
    temp2->digits = malloc(sizeof(int)*MAX50);
    	if (temp2 == NULL){
    		free(temp2);
    		return NULL;
    	}
	temp1 = first;
	temp1->digits = first->digits;
	temp2 = second;
	temp2->digits = second->digits;
    	if ( n == 0 ){
        	return first;
    	}
	    else if (n == 1) {
	        return second;
	    }
	    else {
	        int i;
	        for ( i = 2 ; i <= n ; i++ ) {
	        
	        
	        sum = big50Add(temp1, temp2);
	        temp2 = temp1;
	        temp1 = sum;
	        }
	        return sum;
	    }
}

void big50Rating(void){
	big50RatingStruct *rating = malloc(sizeof(big50RatingStruct));
	rating->NID = malloc(sizeof(char)*8);
	rating->NID = "ni915898";
	rating->degreeOfDifficulty = 4.0;
	rating->duration = 15.0;
	fprintf(stderr, "\n\n\n%s;%f;%f\n\n", rating->NID, rating->degreeOfDifficulty, rating->duration);
	free(rating);
}

//Function to load the string in the file named "inputString" 
Integer50 *loadCryptoVariable(char *inputString){
	
	FILE *crypFile;
	char crypStr[101];
		if (inputString == NULL){
		return NULL;
		}
	crypFile = fopen( inputString , "r");
		if (crypFile == NULL ){
			return NULL;
		}
	if( fgets (crypStr, 100, crypFile)!=NULL ) {
     	Integer50 *x = parseString(crypStr);
	return x;
   }
	fclose(crypFile);
}

//Loads in the seed value for the random number generator for generating the same 50 digit numbers based on seeding value
Integer50 *loadHWConfigVariable(int doSeed){
	Integer50 *x = malloc(sizeof(Integer50));
	x->digits = malloc(MAX50*sizeof(int));
	memset(x->digits, 0, MAX50*sizeof(int));
		int i, j;
	int random[5];
	if ( doSeed >= 1 ) {
		srand(doSeed);
		int k = 0;
		for (i = 0 ; i <= 9 ; i++ ) {
			for (j = 0 ; j <=4 ; j++){
				if ( k >= 50 ){
				return x;
				}
				random[j] = rand() % 10;
				x->digits[k] = random[j];
				k++;
			}
			
		}
	}
	else {
		for (j = 0 ; j <=4 ; j++){
			random[j] = rand() % 10;
		
		}
		int l = 0;
		for (i = 0 ; i <= 9 ; i++ ) {
			for (j = 0 ; j <=4 ; j++){
				if ( l >= 50 ){
				return x;
				}
				x->digits[l] = random[j];
				l++;
			}
		}	
	}
return x;	
}