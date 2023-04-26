#ifndef _REGISTER_H_
#define _REGISTER_H_

/* Sizes */
#define RES_MAX 4096
#define PATH_SIZE 16

/* Stats types */
#define TYPE_STATSTIME 0
#define TYPE_STATSCMD 1
#define TYPE_STATSUNIQ 2

/* Struct */
typedef struct __REGISTER__ *Register;

/* Functions */
int registerProcess(pid_t pid, char exec[], long time, char path[]);
void registerStats(pid_t pid, char type, char filter[], char path[]);

#endif