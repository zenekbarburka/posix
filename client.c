#include <stdio.h>
#include <mqueue.h>
#include <unistd.h>
#include "commons.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>

mqd_t client;
mqd_t server;
char clientpath[20];
char* sending_buffer[MESSAGE_SIZE];

struct mq_attr clients_attributes;

struct mq_attr receiving_attributes;

void connect_to_server(){

    server = mq_open(servers_path,O_WRONLY);
    if(server == -1) {
        printf("connection to server failed\n");

    } else {

        printf("connected to server with id: %d \n",server);
    }
};

void set_clients_attributes(){


    // deal with attributes

    clients_attributes.mq_maxmsg = QUEUE_SIZE;
    clients_attributes.mq_msgsize = MESSAGE_SIZE;

};







void create_clients_queue(){


    // create clients path
    pid_t client_pid = getpid();

    sprintf(clientpath, "/%d", client_pid);


    // open clients queue
    client = mq_open(clientpath,O_RDONLY | O_CREAT | O_EXCL, 0666, &clients_attributes);
   // printf(errno);

    if (client == -1) {
        printf("failes opening client's queue \n");
        exit(-1);
    } else {
        printf("connected to client's queue: %d\n",client);
    }
};

void register_at_server(){

    message message;
    message.mtype = 1;
    int client_pid = getpid();
    message.sender = client_pid;
    char separator = ' ';


    snprintf(sending_buffer,MESSAGE_SIZE,"%ld%c%d",message.mtype,separator,message.sender);


    if ((mq_send(server,sending_buffer, MESSAGE_SIZE,0)) == -1) {
        printf("failed to send registration request\n");
        exit(-1);
    }

    else {

        printf("%s",sending_buffer);
        printf("sent registration request\n");


    }

};

void close_and_unlink_queue(){

    printf("At exit closing and unlinking queue\n");

    mq_close(client);
    mq_unlink(clientpath);

};


void check_attributes(){

    if ((mq_getattr(client,&receiving_attributes)) == -1) {

        printf("cannot read own's queue\n exit \n");
        exit(-1);
    }
};

int check_for_messages_in_the_queue(){

    //printf("checking for messages\n");

    int messages_in_queue;
    messages_in_queue = receiving_attributes.mq_curmsgs;
    //printf("there are %d message in the servers queue\n",messages_in_queue);
    // printf("message in the queue!\n");
    return messages_in_queue;


};

char* receive_message(){

    char *receiving_buffer = malloc(sizeof(char)*MESSAGE_SIZE);

    if ((mq_receive(client,receiving_buffer,MESSAGE_SIZE,NULL))>0){

        return receiving_buffer;
    }

    else {

        printf("Server failed to receive message");

        mq_close(client);
        mq_unlink(clientpath);

        exit(-1);


    }

};



void choose_action(){

    char action_type[1];
    printf("Chose action: T to request time, S to shut down\n");
    scanf(" %c",action_type);
    printf("%s", action_type);











};





int main() {

    mq_close(client);
    mq_unlink(clientpath);

    connect_to_server();

    set_clients_attributes();

    create_clients_queue();

    register_at_server();

    int condition = 1;
    int client_active = 0;

    while (condition) {

        check_attributes();

        if (check_for_messages_in_the_queue() > 0) {
            char *received_message = receive_message();
            if ((strcmp(received_message, "activation")) == 0) {
                client_active = 1;
                condition = 0;
                free(received_message);
            }
        }
    }

    char action_type[1];
    printf("Chose action: T to request time, S to shut down\n");
    scanf(" %c",action_type);
    printf("%s", action_type);









    printf("Hello, World!\n");
        return 0;

}
