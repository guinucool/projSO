#ifndef _UTILS_H_
#define _UTILS_H_

/* Path to FIFO */
#define FIFO_PATH "tmp/serverListen"

/* Program names */
#define SERVER_NAME "monitor"
#define CLIENT_NAME "tracer"

/* Methods */
long getTimeMilliseconds();
void errorHandler(int err, char prog[]);
void argHandler(int err, int arg, char prog[]);

#endif