#ifndef _PROCESS_H_
#define _PROCESS_H_

/* Sizes */
#define EXEC_MAX 4096
#define PATH_SIZE 16

/* Paths */
#define QUEUE_PATH "tmp/queue"

/* Struct */
typedef struct __PROCESS__ *Process;

/* Functions */
void addProcessQueue(pid_t pid, char exec[], long start);
void mapProcessQueue(pid_t pid, char mode);
int executeProcess(char cmd[]);
int processStatusResquest();

#endif