#ifndef _DYNARRAY_H_
#define _DYNARRAY_H_

/* Constructor & Destructor */
char ** createDArray();
void destroyDArray(char ** arr);

/* Functions */
char ** insertDArray(char ** arr, char * str);
char ** stringToDArray(char * str, char * delim);
void darrayToString(char ** arr, char * str, char * delim, int size);
int checkIfInDArray(char ** arr, char * str);
int stringToMultipleDArray(char ** arr[], int N, char * str, char * delim1, char * delim2);
void destroyMultipleDArray(char ** arr[], int N);
void multipleToString(char ** arr[], int N, char * dest, char * delim, int size);

#endif