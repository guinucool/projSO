#ifndef _DYNARRAY_H_
#define _DYNARRAY_H_

/* Constructor & Destructor */
char ** createDArray();
void destroyDArray(char ** arr);

/* Functions */
char ** insertDArray(char ** arr, char * str);
char ** stringToDArray(char * str, char * delim);
int checkIfInDArray(char ** arr, char * str);

#endif