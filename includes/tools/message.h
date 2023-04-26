#ifndef _MESSAGE_H_
#define _MESSAGE_H_

/* Sizes */
#define MSG_SIZE 4096

/* Message types */
#define TYPE_PROCESS_START 'P'
#define TYPE_PROCESS_END 'E'
#define TYPE_PROCESS_FAIL 'F'

#define TYPE_STATUSREQUEST 'S'
#define TYPE_STATSTIMEREQUEST 'T'
#define TYPE_STATSCMDREQUEST 'C'
#define TYPE_STATSUNIQREQUEST 'U'
#define TYPE_REPLY 'R'

/* Default messages */
#define MSG_CONTENT_EMPTY "empty"

/* Struct */
typedef struct __MESSAGE__ *Message;

/* Senders and listeners */
int messageListen(int listener, void (*printer)(Message), char * pidpath);
int messageSend(pid_t pid, char type, char content[], long time, int fifo);

/* Requester */
int messageResquest(char type, char * argv[], int argc, void (*printer)(Message));

/* Printers */
void printDebugMessage(Message msg);
void printStatusMessage(Message msg);
void printStatsTimeMessage(Message msg);
void printStatsCmdMessage(Message msg);
void printStatsUniqMessage(Message msg);

#endif