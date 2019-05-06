#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 500

#ifndef DEBUG
#define DEBUG 0
#endif

/* 
Nicholas Poidomani
Ni915898
This is my own code, besides the templates given to us my Prof. McAlpin


Recap left & right rotations (simple case)
T1, T2 and T3 are subtrees of the tree rooted with y (on left side) 
or x (on right side)           
                y                               x
               / \     Right Rotation          /  \
              x   T3   – – – – – – – >        T1   y 
             / \       < - - - - - - -            / \
            T1  T2     Left Rotation            T2  T3
Keys in both of the above trees follow the following order 
      keys(T1) < key(x) < keys(T2) < key(y) < keys(T3)
So BST property is not violated anywhere.
*/

typedef struct airPdata{
int seqNumber;
char *LocID; // Airport's "Short Name"
char *fieldName; // Airport Name
char *city; // Associated City
float longitude; //long before lat
float latitude;//lat
} airPdata;
typedef struct node
{
    int key;
    airPdata *airport;
    struct node *left;
    struct node *right;
    int height;
} node;

void printNode(node *N)
{
    if(DEBUG) printf("%s\n", __FUNCTION__);

    if(N == NULL)
        return;

    airPdata *data = N->airport;

    printf("%s,%s,%s,%.4f,%.4f\n",data->LocID,data->fieldName, data->city,
                                  data->latitude, data->longitude);
}

void inOrder(node *N)
{
    if(DEBUG) printf("%s\n", __FUNCTION__);
    
    if(N == NULL)
        return;

    inOrder(N->left);
    printNode(N);
    inOrder(N->right);
}
 
// A utility function to get height of the tree
int height(node *N)
{
    if(DEBUG) printf("%s\n", __FUNCTION__);
    
    if (N == NULL)
        return 0;
    return N->height;
}
 
// A utility function to get maximum of two integers
int max(int a, int b)
{
    if(DEBUG) printf("%s\n", __FUNCTION__);
    
    return (a > b)? a : b;
}
 
/* Helper function that allocates a new node with the given key and
    NULL left and right pointers. */
node* newNode(airPdata *airport)
{
    if(DEBUG) printf("%s\n", __FUNCTION__);
    
    node* N = malloc(sizeof(node));

    N->key   = airport->seqNumber;
    N->airport = airport;
    N->left   = NULL;
    N->right  = NULL;
    N->height = 1;  // new node is initially added at leaf
    return(N);
}
 
// A utility function to right rotate subtree rooted with y
// See the diagram given above.
node *rightRotate(node *y)
{
    if(DEBUG) printf("%s\n", __FUNCTION__);
    
    struct node *x = y->left;
    struct node *T2 = x->right;
 
    // Perform rotation
    x->right = y;
    y->left = T2;
 
    // Update heights
    y->height = max(height(y->left), height(y->right))+1;
    x->height = max(height(x->left), height(x->right))+1;
 
    // Return new root
    return x;
}
 
// A utility function to left rotate subtree rooted with x
// See the diagram given above.
node *leftRotate(node *x)
{
    if(DEBUG) printf("%s\n", __FUNCTION__);
    
    node *y = x->right;
    node *T2 = y->left;
 
    // Perform rotation
    y->left = x;
    x->right = T2;
 
    //  Update heights
    x->height = max(height(x->left), height(x->right))+1;
    y->height = max(height(y->left), height(y->right))+1;
 
    // Return new root
    return y;
}
 
/* 
 * RECAP Balance is based on Height
 *     Hn = Hl - Hr 
 * so 
 *    positive => LEFT HEAVY
 *    negative => RIGHT HEAVY
 */
// Get Balance factor of node N
int getBalance(node *N)
{
    if(DEBUG) printf("%s\n", __FUNCTION__);
    
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}
 
node* insert(node* N, airPdata *airport)
{
    if(DEBUG) printf("%s\n", __FUNCTION__);

    int key = airport->seqNumber;

    /* 1.  Perform the normal BST insertion */
    if (N == NULL)
        return(newNode(airport));
 
    if (key < N->key)
        N->left  = insert(N->left, airport);
    else
        N->right = insert(N->right, airport);
 
    /* 2. Update height of this ancestor node */
    N->height = max(height(N->left), height(N->right)) + 1;
 
    /* 3. Get the balance factor of this ancestor node to check whether
       this node became unbalanced */
    int balance = getBalance(N);
 
    // If this node becomes UNBALANCED, then there are 4 cases
    
    /* CASE # 1 => LEFT-LEFT aka left?
       T1, T2, T3 and T4 are subtrees.
         z                                      y 
        / \                                   /   \
       y   T4      Right Rotate (z)          x      z
      / \          - - - - - - - - ->      /  \    /  \ 
     x   T3                               T1  T2  T3  T4
    / \
  T1   T2
     */
    // Left Left Case in code
    if (balance > 1 && key < N->left->key)
        return rightRotate(N);
    
    /* Case #2 => RIGHT-RIGHT aka right?

       z                                y
      /  \                            /   \ 
     T1   y     Left Rotate(z)       z      x
         /  \   - - - - - - - ->    / \    / \
	T2   x                     T1  T2 T3  T4
       / \
     T3  T4

     */
    // Right Right Case in code
    if (balance < -1 && key > N->right->key)
        return leftRotate(N);

    
    /* CASE # 3 => LEFT-RIGHT aka left-right?
     z                               z                           x
    / \                            /   \                        /  \ 
   y   T4  Left Rotate (y)        x    T4  Right Rotate(z)    y      z
  / \      - - - - - - - - ->    /  \      - - - - - - - ->  / \    / \
T1   x                          y    T3                    T1  T2 T3  T4
    / \                        / \
  T2   T3                    T1   T2

    */
    // Left Right Case in code
    if (balance > 1 && key > N->left->key)
    {
        N->left =  leftRotate(N->left);
        return rightRotate(N);
    }
    /* CASE #4 = RIGHT-LEFT aka right-left?
        z                            z                            x
       / \                          / \                          /  \ 
      T1   y   Right Rotate (y)    T1   x      Left Rotate(z)   z      y
      / \   - - - - - - - - ->     /  \      - - - - - - - ->  / \    /	\
    x   T4                       T2    y                      T1  T2  T3  T4
   / \                                /  \
 T2   T3                             T3   T4   
     */
    // Right Left Case in code
    if (balance < -1 && key < N->right->key)
    {
        N->right = rightRotate(N->right);
        return leftRotate(N);
    }
 
    /* return the (unchanged) node pointer */
    return N;
}
 
// A utility function to print preorder traversal of the tree.
// The function also prints height of every node
void preOrder(node *root)
{
    if(DEBUG) printf("%s\n", __FUNCTION__);
    
    if(root != NULL)
    {
      printf("%2d - %s\n", root->key, root->airport->LocID);
        preOrder(root->left);
        preOrder(root->right);
    }
}

void freeNodes(node *head)
{
    if(DEBUG) printf("%s\n", __FUNCTION__);
    
    if(head == NULL)
        return;

    freeNodes(head->left);
    freeNodes(head->right);

    free(head);
}
void parseLine(char *line, airPdata *apd);
void printData(int length, airPdata *apd);
void deleteStruct(airPdata *apd);
float sexag2decimal(char *degreeString);
void sortByLocID(airPdata *airports);
void sortByLatitude(airPdata *airports);

// List size
int count;

int main (int argc, char *argv[])
{
    // Declare input buffer and other parameters
    FILE *fid;
    char buffer[BUFFER_SIZE];
    count = 0;

    char sort;
    
    // Check for command line input and open input file.
    if(argc == 3){
        fid = fopen(argv[1], "r");
        if(fid==NULL){
            printf("File %s failed to open. Aborting.\n", argv[1]);
            return 2;
        }

        // a for alphabetical sort, n for south-to-north sort
        sort = argv[2][0];

        if((sort != 'a' && sort != 'd') || strlen(argv[2]) > 1 ) {
            printf("Invalid sort parameter. Use 'a' or 'd'.\n");
            return 3;
        }
    }
    else{
        printf("Incorect number of input parameters. Please specify the name of the input file only.\n");
        printf("Syntax: ./hw1ecl.exe [input file]\n");
        return 1;
    }

    int header = 0;
    
    if(fgets(buffer, BUFFER_SIZE, fid) == NULL)
        return 5;

    if(strncmp(buffer, "code,", 5) == 0)
        header = 1;
    else
        count++;

    // Determine length of the file.
    while(fgets(buffer, BUFFER_SIZE, fid) != NULL){
        count++;
    }
    rewind(fid);
    
    
    // Declare a struct array and allocate memory.
    airPdata *data;

    data = malloc(sizeof(airPdata)*count);
    if(data==NULL){
        printf("Memory allocation for airPdata array failed. Aborting.\n");
        return 2;
    }

    // Read and parse each line of the inputt file.
    if(header)
        fgets(buffer, BUFFER_SIZE, fid);

    int i;
    for(i = 0; i<count; i++){
        fgets(buffer, BUFFER_SIZE, fid);
        
        // fgets() includes the New Line delimiter in the output string. 
        // i.e. "This is my string.\n\0"
        // We will truncate the string to drop the '\n' if it is there.
        // Note: There will be no '\n' if the line is longer than the buffer.
        if(buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer)-1] = '\0';
        
        parseLine(buffer, data+i);
    }

    if(sort == 'a') {
        sortByLocID(data);
    } else {
        sortByLatitude(data);
    }

    node *head = NULL;
    int key = 0;

    for(i=0; i<count; i++)
    {
        (data+i)->seqNumber = -1;

        int len = strlen((data+i)->LocID);
        if(len < 3 || len > 4)
            continue;

        if((data+i)->LocID[0] == 'X')
            continue;

        if((data+i)->LocID[0] == 'F' && (data+i)->LocID[1] == 'L')
            continue;

        if( isdigit((data+i)->LocID[0]) )
            continue;

        (data+i)->seqNumber = key;
        key++;

        head = insert(head, &(data[i]));
    }

    // close the input file.
    fclose(fid);

    printf("code,name,city,lat,lon\n");
    inOrder(head);

    // Free the memory used for fields of the structs.
    for(i=0; i<count; i++){
        deleteStruct(data+i);
    }

    freeNodes(head);

    // Free the memory for the struct array.
    free(data);

    return 0;

}

int alphaOrdered(char *a, char* b)
{
    int aFirst = 1;

    int aLen = strlen(a);
    int bLen = strlen(b);

    int i;
    for(i=0; i<aLen; i++) {
        if(i == bLen)
            return 1;

        if(a[i] == b[i])
            continue;

        if(a[i] < b[i])
            return 1;

        return 0;
    }
}

void sortByLocID(airPdata *airports)
{
    if(DEBUG) printf("%s\n", __FUNCTION__);

    int i, j;
    for(i=0; i<count; i++)
        for(j=0; j<count-i-1; j++)
        {
            // if out of order
            if(!alphaOrdered(airports[j].LocID, airports[j+1].LocID))
            {
                // swap
                airPdata temp = airports[j];
                airports[j] = airports[j+1];
                airports[j+1] = temp;
            }
        }
}

void sortByLatitude(airPdata *airports)
{
    if(DEBUG) printf("%s\n", __FUNCTION__);

    int i, j;
    for(i=0; i<count; i++)
        for(j=0; j<count-i-1; j++)
        {
            // if out of order
            if(airports[j].latitude > airports[j+1].latitude)
            {
                // swap
                airPdata temp = airports[j];
                airports[j] = airports[j+1];
                airports[j+1] = temp;
            }
        }
}

void parseLine(char *line, airPdata *apd)
{
    if(DEBUG) printf("%s[%s]\n", __FUNCTION__, line);

    int i=0, j=0, k=0, commas=0;

    int total_commas = 0;

    while(*(line+k) != '\0')
    {
        if(*(line+k) == ',') {
            total_commas++;
        }
        k++;
    }

    int col_locID;
    int col_fieldName;
    int col_city;
    int col_lat;
    int col_long;

    int short_format = 0;

    if(total_commas < 5) {
        short_format = 1;

        col_locID       = 0;
        col_fieldName   = 1;
        col_city        = 2;
        col_lat         = 3;
        col_long        = 4;
    }
    else {
        col_locID       = 1;
        col_fieldName   = 2;
        col_city        = 3;
        col_lat         = 8;
        col_long        = 9;
    }
    
    while(commas <= col_long) {

        if(!short_format || commas != col_long) {
            while(*(line+i)!=','){
                i++;
            }
            // strncpy does not append a '\0' to the end of the copied sub-string, so we will
            // replace the comma with '\0'. 
            *(line+i) = '\0';
        }

        if(commas == col_locID) {   //Grab the second "field" - Location ID
            apd->LocID = malloc(sizeof(char)*(i-j+1));
            if(apd->LocID==NULL){
                printf("malloc failed to initialize airPdata.LocID.\n");
                exit(-1);
            }
            strncpy(apd->LocID, line+j, i-j+1);
        }

        else if(commas == col_fieldName) {   //Grab the third "field" - Field Name
            apd->fieldName = malloc(sizeof(char)*(i-j+1));
            if(apd->fieldName==NULL){
                printf("malloc failed to initialize airPdata.fieldName.\n");
                exit(-1);
            }
            strncpy(apd->fieldName, line+j, i-j+1);
        }

        else if(commas == col_city) {   //Grab the fourth "field" - City
            apd->city = malloc(sizeof(char)*(i-j+1));
            if(apd->city==NULL){
                printf("malloc failed to initialize airPdata.city.\n");
                exit(-1);
            }
            strncpy(apd->city, line+j, i-j+1);
        }

        else if(commas == col_lat) {   //Grab the ninth "field" - Latitude (sexagesimal string)
            if(short_format)
                sscanf(line+j, "%f", &(apd->latitude) );
            else
                apd->latitude = sexag2decimal(line+j);

        }

        else if(commas == col_long) {   //Grab the tenth "field" - Longitude (sexagesimal string)
            if(short_format)
                sscanf(line+j, "%f", &(apd->longitude) );
            else
                apd->longitude = sexag2decimal(line+j);
        }

        j=++i;
        commas++;
    }
}

void printData(int length, airPdata *data)
{
    if(DEBUG) printf("%s\n", __FUNCTION__);
    
    printf("code,name,city,lat,lon\n");

    int i;
    for(i=0; i<length; i++) {
        printf("%s,%s,%s,%.4f,%.4f\n",(data+i)->LocID,(data+i)->fieldName, (data+i)->city,
                                      (data+i)->latitude, (data+i)->longitude);
    }
}

void deleteStruct(airPdata *apd)
{
    if(DEBUG) printf("%s\n", __FUNCTION__);
    
    free(apd->city);
    free(apd->fieldName);
    free(apd->LocID);
}

float sexag2decimal(char *degreeString)
{
    if(DEBUG) printf("%s[%s]\n", __FUNCTION__, degreeString);

    if(!degreeString)
        return 0.0f;

    float decimal = 0.0f;

    int degrees;
    int minutes;
    int seconds;
    int millis;
    int sign;
    sscanf(degreeString, "%d-%d-%d.%d", &degrees, &minutes, &seconds, &millis);

    if(minutes < 0 || minutes > 59  ||
       degrees < 0 || degrees > 180 ||
       seconds < 0 || seconds > 59  ||
       millis  < 0 || millis  > 9999 )
    {
        return 0.0f;
    }

    int len = strlen(degreeString);
    int direction = degreeString[len-1];

    if(direction == 'N' || direction == 'E')
        sign = 1.0f;
    else if(direction == 'S' || direction == 'W')
        sign = -1.0f;
    else
        return 0.0f;

    decimal += degrees;
    decimal += ((float) minutes) / 60.0f;
    decimal += ((float) seconds) / (60.0f * 60.0f);
    decimal += ((float) millis) / (60.0f * 60.0f * 1000.0f);
    decimal *= sign;

    return decimal;
}
