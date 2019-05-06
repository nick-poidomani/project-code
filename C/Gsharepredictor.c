#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int poweroftwo(int exponent);
int main(int argc, char* argv[]){
    double total;
    int correctpredictions = 0, incorrectpredictions = 0;
    char prediction = 0; //initialze the prediction character
    int M = atoi(argv[1]); // get the number of the PC bits used to index the gshare table
    int N = atoi(argv[2]); //get the number of bits in global history register used to index gshare table
    FILE *filepassed = fopen(argv[3], "r"); // open trace file for reading only
    //make sure m>=n it is a requirement for gshare
    if(N>M){
        printf("value of n>m, this does not meet requirements\n");
        exit(1);
    }
    /*allocate the table of 2^m to store state
    of 2 bit predictors*/
    unsigned long long int *ARRAY = malloc(poweroftwo(M)*sizeof(unsigned long long int)); 
    
    //initialize the table elements to WEAKLY TAKEN(2) before parsing
    int i = 0;
    for(i=0; i<=poweroftwo(M); i++){
        ARRAY[i] = 2; //initialize the element in the array to weakly taken
    }
    //create the global history register of n bits in length
    unsigned long long int GBH; 
	
    // if our parameter N is 1 we have a special case, initialize the GBH to 1
	if(N==1){ 
        GBH = 1;
    } else GBH = 2; 
    char buffer[100]; //buffer to parse the file 
	unsigned long long int PC_ADDRESS_ORIGINAL; //untrimmed PC address
    
    // now we read the file line by line
    while(fgets(buffer, 100, filepassed)){
		//parse the current line
		while(sscanf(buffer, "%llx %c", &PC_ADDRESS_ORIGINAL, &prediction)!=2){
            if(fgets(buffer, 100, filepassed)==NULL)
				break; //means that we are at the end of the file 
            } //end of inner while loop
       
        //TRIM LOWER TWO BITS OF PC THATS PARSED FROM FILE
        unsigned long long int TRIMMED_PC = PC_ADDRESS_ORIGINAL>>2; //trimmed PC
        unsigned long long int M_GBH = 0;//create the M_GBH
        M_GBH = GBH * poweroftwo(M-N); //extend the nbit global branch history to m bits
       unsigned long long int M_PC = TRIMMED_PC % poweroftwo(M); // remainder by division gets the first mbits of PC
       unsigned long long int index;
       if(N > 0){ // if there is a global history register this is going to be the index
       index = M_PC ^ M_GBH; // get the index you need to access by exclusive OR operation
       } else index = M_PC ^ 0;// if there is no global history register then the index is just modpc ^ 0
        //now we go to index in table and determine if its preciction is correct with actual parse
        if(ARRAY[index] == 3){ //if prediction is strongly taken
            if(prediction == 't'){ //if prediction is correct
                ARRAY[index] = 3; // update the prediction
                correctpredictions++; //increment prediction outcome count
            } else { //otherwise prediction is not correct
                ARRAY[index] = 2; // update the prediction
                incorrectpredictions++; //increment prediction outcome count
            }
        } else if(ARRAY[index] == 2){ //if prediction is weakly taken
            if(prediction == 't'){ //if prediction is correct
                ARRAY[index] = 3; // update the prediction
                correctpredictions++; //increment prediction outcome count
            } else { //otherwise prediction is not correct
                ARRAY[index] = 1; // update the prediction
                incorrectpredictions++; //increment prediction outcome count
            }
        } else if(ARRAY[index] == 1){ //if prediction is weakly not taken
            if(prediction == 'n'){ //if prediction is correct
                ARRAY[index] = 0; // update the prediction 
                correctpredictions++; //increment prediction outcome count
            } else { //otherwise prediction is not correct
                ARRAY[index] = 2; // update the prediction
                incorrectpredictions++; //increment prediction outcome count
            }

        } else if(ARRAY[index] == 0){ //if prediction is strongly not taken
            if(prediction == 'n'){ //if prediction is correct
                ARRAY[index] = 0; // update the prediction
                correctpredictions++; //increment prediction outcome count
            } else { //otherwise prediction is not correct
                incorrectpredictions++; //increment prediction outcome count
                ARRAY[index] = 1; // update the prediction
            } 
        }
        
    if(N>0){ //if there is a Global History register
     GBH = GBH >> 1; // remove least significant bit of the gbh
     GBH = GBH & (poweroftwo(N-1)-1); // clear the most significant bit of the gbh
    if(prediction == 't'){
         GBH =  GBH | poweroftwo(N-1); //add the prediction to the gbh most significant bit
    }
    }
        
    //otherwise if the outcome was not taken do nothing to GBH
    } // end of while loop


total = correctpredictions+incorrectpredictions; //total accesses equals sum of correct and incorrect predictions
double predictionaccuracy = (double)(incorrectpredictions/total);
//print all information
printf("Configuration: m = %d, n = %d \nTrace file: %s \n",M,N,argv[3]); //output the general information about predictor
printf("Misprediction ratio = %lf\n", predictionaccuracy); //print the misprediction ratio
printf("Misprediction rate = %.2lf %%\n",(predictionaccuracy*100.000)); //print the misprediction rate

} //end of main 
 
//function to calculate 2^x
int poweroftwo(int exponent){
    int j = 0;
    int result = 1;
    if(exponent == 0){
        return 1;
    }else{
        for(j = 1; j<=exponent; j++){
        result*=2;//multiply by 2 however many times the exponent is 
        }     
    return result;
    }
}