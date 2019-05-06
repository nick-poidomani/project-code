#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "airPdata.h"
void parseLine(char *line, airPdata *apd);
void printData(int length, airPdata *apd);
void deleteStruct(airPdata *apd);
double sexag2decimal(char *degreeString);

#define BUFFER_SIZE 500

int main (int argc, char *argv[]){


	// Declare input buffer and other parameters
	FILE *fid;
	char buffer[BUFFER_SIZE];
	int count = 0;
	// Check for command line input and open input file.
	if(argc == 2){
		fid = fopen(argv[1], "r");
		if(fid==NULL){
			printf("File %s failed to open.\n", argv[1]);
			return 2;
		}
	}
	else{
		printf("Incorect number of input parameters. Please specify the name of the input file only.\n");
		printf("Syntax: ./hw1ecl.exe [input file]\n");
		return 1;
	}
	
	// Determine length of the file.
	while(fgets(buffer, BUFFER_SIZE, fid) != NULL){
		count++;
	}
	rewind(fid);
	
	// Declare a struct array and allocate memory.
	airPdata *data;
	data = malloc( sizeof(airPdata)*count );
	if(data == NULL){
		printf( "Memory allocation for airPdata array failed. Aborting.\n" );
		return 2;
	}
	// Read and parse each line of the inputt file.
	for( int i = 0; i < count; i++ ){
		fgets(buffer, BUFFER_SIZE, fid);
		// fgets() includes the New Line delimiter in the output string. 
		// i.e. "This is my string.\n\0"
		// We will truncate the string to drop the '\n' if it is there.
		// Note: There will be no '\n' if the line is longer than the buffer.
		if(buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer)-1] = '\0';
		parseLine(buffer, data+i);
	}
	// close the input file.
	fclose(fid);
	// Output the data to stdout.
	printData(count, data);
	// Free the memory used for fields of the structs.
	for(int i=0; i<count; i++){
		deleteStruct(data+i);
	}
	// Free the memory for the struct array.
	free(data);
	return 0;
}
void parseLine(char *line, airPdata *apd){

	int i=0, j=0, commas=0;
	char *latstr, *longstr;
	
	while(commas<15){
		while(*(line+i)!=','){
			i++;
		}
		
		// strncpy does not append a '\0' to the end of the copied sub-string, so we will
		// replace the comma with '\0'. 
		*(line+i) = '\0';

		switch (commas){

		case 1:   //Grab the second "field" - Location ID
			apd->LocID = malloc(sizeof(char)*(i-j+1));
			if(apd->LocID==NULL){
                        	printf("malloc failed to initialize airPdata.LocID.\n");
                                exit(-1);
                        }
                        strncpy(apd->LocID, line+j, i-j+1);
                        break;
		case 2:   //Grab the third "field" - Field Name
			apd->fieldName = malloc(sizeof(char)*(i-j+1));
			if(apd->fieldName==NULL){
				printf("malloc failed to initialize airPdata.fieldName.\n");
				exit(-1);
			}
			strncpy(apd->fieldName, line+j, i-j+1);
			break;
		case 3:   //Grab the fourth "field" - City
			apd->city = malloc(sizeof(char)*(i-j+1));
			if(apd->city==NULL){
				printf("malloc failed to initialize airPdata.city.\n");
				exit(-1);
			}
			strncpy(apd->city, line+j, i-j+1);
			break;

		case 8:   //Grab the ninth "field" - Latitude (sexagesimal string)
			
			latstr = malloc(sizeof(char)*(i-j+1));
			strncpy(latstr, line+j, i-j+1);
			apd->latitude = sexag2decimal (latstr);
			free(latstr);
			break;
		case 9:   //Grab the tenth "field" - Longitude (sexagesimal string)
			longstr = malloc(sizeof(char)*(i-j+1));
			strncpy(longstr, line+j, i-j+1);
			apd->longitude = sexag2decimal (longstr);
			free(longstr);
			break;
		}

		j=++i;
		commas++;
	}
}

void printData(int length, airPdata *data){
	printf("%s,%s,%s,%s,%s\n",  "code", "name", "city", "lat", "lon");
	for(int i=0; i<length; i++){
		printf("%s,%s,%s,%.4f,%.4f\n",(data+i)->LocID,(data+i)->fieldName,
		  (data+i)->city,(data+i)->latitude,(data+i)->longitude);
	}
}

void deleteStruct(airPdata *apd){
	free(apd->city);
	free(apd->fieldName);
	free(apd->LocID);

}

//converts the longitude/latitude in string format to decimal value
double sexag2decimal (char *degreeString) {
	
		char *pch;
		double sum = 0;
		double d, min, sec;
		
		int len;
		len = strlen(degreeString);
		char copy[ len+1 ];
		strcpy(copy, degreeString);
		
		if (len == 15){ 
			pch = strtok (copy ," ,.-");
			for ( int f = 0 ; f < 4 ; f ++){
				
				if (f == 0 ){
					d = atol(pch);
					if (d > 180){
						return 0.0;
					}
					sum = sum + d;
					pch = strtok (NULL, " ,.-");
				 }
				 if (f == 1 ){
					d = atol(pch);
					if (d > 59 || d < 0 ){
						return 0.01;
					}
					sum = sum + (d / 60.0);
					pch = strtok (NULL, " ,.-");
				 }
				 if (f == 2 ){
					min = atol(pch);
					pch = strtok (NULL, " ,.-");
				 }
				 if (f == 3){
					sec =  atol(pch);
					sec = sec / 1000;
					min = min + sec;
					min = min / 3600;
					sum = sum + min;
				 }
				 
				}
				
				if ( degreeString[14] == 87 ){
						return sum * -1.0;
					 }
				else if ( degreeString[14]== 69){
					return sum;
				}
			return 0.0;
		}
		else if ( len == 14 ) {
		pch = strtok ( copy ," ,.-");
			for ( int f = 0 ; f < 4 ; f ++)
			{
			
			if (f == 0 ){
			 	d = atol(pch);
			 	sum = sum + d;
			 	pch = strtok (NULL, " ,.-");
			 }
			 if (f == 1 ){
			 	d = atol(pch);
			 	sum = sum + (d / 60.0);
			 	pch = strtok (NULL, " ,.-");
			 }
			 if (f == 2 ){
			 	min = atol(pch);
				pch = strtok (NULL, " ,.-");
			 }
			 if (f == 3){
			 	sec =  atol(pch);
			 	sec = sec / 1000;
			 	min = min + sec;
			 	min = min / 3600;
			 	sum = sum + min;
			 }
			 
			}//for loop for latitude/longitude
			if ( degreeString[13] == 78 ){ //check for N/S character w/ ascii code
				return sum;
			}
			else if (degreeString[13] == 83) {
				return sum * -1.0;
			}
		return 0.0;
		}
	
}

/*“My statement that this program is entirely your my own 
work and that you have neither developed your code tog
ether with any another person, nor copied program code 
from any other person, nor permitted your code to be co
pied or otherwise used by any other person, nor have y
ou copied, modiﬁed, or otherwise used program code tha
t you have found in any external source, including bu
t not limited to, online sources”*/ 


