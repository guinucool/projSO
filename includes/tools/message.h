#ifndef _MESSAGE_H_
#define _MESSAGE_H_

/* Sizes */
#define MSG_SIZE 4096

/* Message types */
#define TYPE_PROCESS_START 'P'
#define TYPE_PROCESS_END 'E'
#define TYPE_STATUSREQUEST 'S'

/* Struct */
typedef struct __MESSAGE__ *Message;

/* Senders and listeners */
int messageListen(int listener);
int messageSend(pid_t pid, char type, char content[], long time, char fifo[]);

#endif