#ifndef _PROCESS_H_
#define _PROCESS_H_

/* Sizes */
#define EXEC_MAX 4096

/* Struct */
typedef struct __PROCESS__ *Process;

/* Functions */
int executeProcess(char cmd[]);

#endif