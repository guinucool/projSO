#ifndef _UTILS_H_
#define _UTILS_H_

/* Path to FIFO */
#define FIFO_PATH "tmp/serverListen"

/* Program names */
#define SERVER_NAME "monitor"
#define CLIENT_NAME "tracer"

/* Functions */
long getTimeMilliseconds();
void errorHandler(int err, char prog[]);
void argHandler(int err, int arg, char prog[]);
void errorChildHandler(int err, char prog[]);
void destroyCharArr(char ** arr);
char ** getArgv(char cmd[]);

#endif