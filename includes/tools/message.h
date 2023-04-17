#ifndef _MESSAGE_H_
#define _MESSAGE_H_

/* Sizes */
#define MSG_SIZE 4096

/* Message types */
#define TYPE_PROCESS 'P'
#define TYPE_STATUSREQUEST 'S'

/* Struct */
typedef struct __MESSAGE__ *Message;

/* Functions */
Message createMessage(pid_t pid, char type, char msg[], long time);
void destroyMessage(Message msg);

/* Senders and listeners */
int messageListen();
int messageSend(Message msg);

#endif