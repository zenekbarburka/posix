#ifndef SERVER_COMMONS_H
#define SERVER_COMMONS_H

#include <signal.h>

// define values of server queue attributes

//define message struct

typedef struct messgae {
    char content[4096];
    pid_t sender;
    long mtype;

} message;



#define QUEUE_SIZE 10
#define MESSAGE_SIZE sizeof(message)

// define servers path
const char servers_path[] = "/server";






#endif //SERVER_COMMONS_H
