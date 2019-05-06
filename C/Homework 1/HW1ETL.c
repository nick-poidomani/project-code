/*Assignment 1 
COP3502C Spring 2016
Nicholas Poidomani
Ni915898*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "airPdata.h"
airPdata a;


void main(int argc, char **argv) {
	int i;
	for (i = 0; i < argc; i++) {
		printf("argument %d: %s\n", i, argv[i]);
	}
	printf("\n");
	char* fname = argv[1];
	FILE* file = fopen(fname, "r");
	if (file == 0) {
		printf("could not open file\n");

	}
	else {
		printf("file opened successfully\n");
                
                char *str;
                char* token;
                char* arr[19];
                rewind(file);
                printf("%15s%10s%42s%40s%4s%16s%16s%5s\n", "FAA Site#", "Short Name", "Airport Name", "City", "ST", "Latitude", "Longitude", "Tower");
                printf("%15s%10s%42s%40s%4s%16s%16s%5s\n", "=========", "==========", "============", "====", "==", "========", "=========", "=====");
                while ( fgets(str, 1000, file) != NULL){
                int n = 0;    
                arr[n] = str;
                token = strtok(arr[n], ",");
                a.siteNumber = token;
                token = strtok(NULL, ",");
                a.LocID = token;
                token = strtok(NULL, ",");
                a.fieldName = token;
                token = strtok(NULL, ",");
                a.city = token;
                
                token = strtok(NULL, ",");
                a.state = token;
                int i = 0;
                for (i ; i < 4;  i++){
                    token = strtok(NULL,",");
                }
                a.latitude = token;
                token = strtok(NULL, ",");
                a.longitude = token;
                int b = 0;
                for (b ; b < 3;  b++){
                    token = strtok(NULL,",");  
                }
                a.controlTower = token;
                
                printf("%15s%10s%42s%40s%4s%16s%16s%3s\n",a.siteNumber, a.LocID,a.fieldName,a.city ,a.state ,a.latitude ,a.longitude, a.controlTower );
                
                n++;
                }
                
        }
                               
                fclose(file);
	
        
}

