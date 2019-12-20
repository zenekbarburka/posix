#include <stdio.h>
#include "mqueue.h"
#include "commons.h"
#include "errno.h"
#include "stdlib.h"
#include <string.h>
#include <time.h>


int status_closing = 0;

void send_time(char* clients_pid);
void send_activation(char* clients_pid);


//VARIABLES related to SERVERS QUEUE

mqd_t server; // server


struct mq_attr servers_attributes; // server creation attributes

struct mq_attr receiving_attributes; // server receiving attributes

// set up attributes

void set_servers_attributes(){
    // set up server's attributes
    servers_attributes.mq_maxmsg = QUEUE_SIZE;
    servers_attributes.mq_msgsize = MESSAGE_SIZE;
    printf("attributes set \n");
};

// open server

void open_servers_queue() {

    server = mq_open (servers_path,
                      O_CREAT | O_RDWR | O_EXCL ,
                      0666, &servers_attributes);


    if (server == -1) {
        printf("failed to open server's queue\n");
        printf(errno);
        exit(-1);
    } else {
        printf("opened servers queue as: %d\n",server);


    }
};

// check attributes

void check_attributes(){

    if ((mq_getattr(server,&receiving_attributes)) == -1) {

        printf("cannot read server's queue\n exit \n");
        exit(-1);
    }
};

void close_and_unlink_queue(){

    printf("At exit closing and unlinking queue\n");

    mq_close(server);
    mq_unlink(servers_path);

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

    if ((mq_receive(server,receiving_buffer,MESSAGE_SIZE,NULL))>0){

        return receiving_buffer;
    }

    else {

        printf("Server failed to receive message");

        mq_close(server);
        mq_unlink(servers_path);

        exit(-1);


    }

};


void respond(char *message_type, char* clients_pid) {

    printf("responding\n");

    char* type_one = "1";
    char* type_two = "2";
    char *type_three ="3";

    if (strcmp(message_type, type_one) == 0) {

        printf("TYPE 1\n");

        send_activation(clients_pid);

    }

    if (strcmp(message_type, type_two) == 0) {

        printf("TYPE 2\n");

       send_time(clients_pid);


    }

    if (strcmp(message_type, type_three) == 0) {

        printf("type 3 - SHUTDOWN INITIATED\n");

    }

}

void send_time(char* clients_pid) {

    time_t mytime = time(NULL);
    char *time_str = ctime(&mytime);
    time_str[strlen(time_str)-1] = '\0';





    char *clientpath[20];
    int clients_pid_int = atoi(clients_pid);
    sprintf(clientpath,"/%d",clients_pid_int);
    printf("clients path: %s\n",clientpath);


    mqd_t  client;
    client = mq_open(clientpath,O_RDWR , 0666, &servers_attributes);

    if (client == -1) {
        printf("failes opening client's queue \n");
        exit(-1);
    } else {
        printf("connected to client's queue: %d\n",client);
    }



};


void send_activation(char* clients_pid) {

    char *clientpath[20];
    int clients_pid_int = atoi(clients_pid);
    sprintf(clientpath,"/%d",clients_pid_int);
    printf("clients path: %s\n",clientpath);


    mqd_t  client;
    client = mq_open(clientpath,O_RDWR , 0666, &servers_attributes);

    if (client == -1) {
        printf("failes opening client's queue \n");
        exit(-1);
    } else {
        printf("connected to client's queue: %d\n",client);
        char* activation = malloc(sizeof(char)*MESSAGE_SIZE);
        char* activation_literal = "activation";
        sprintf(activation,"%s",activation_literal);

        int message_sent = mq_send(client,activation,MESSAGE_SIZE,0);
        printf("message sent with: %d",message_sent);





    }
};


int main() {

    // clean remainings of previous trials

    mq_close(server);
    mq_unlink(servers_path);

    // define atexit behaviour

    atexit(close_and_unlink_queue);

    //set servers attributes:

    set_servers_attributes();

    // open server's queue

    open_servers_queue();

    // receiving messages in the loop

    int condition = 1;

    while (1) {

        check_attributes();

        if (check_for_messages_in_the_queue() > 0) {

            char *received_message = receive_message();

            char *tok_one = strtok(received_message," ");
            char *tok_two = strtok(NULL, " ");
            //int clients_pid = atoi(tok_two);

            respond(tok_one,tok_two);

        } else if ((check_for_messages_in_the_queue() ==0) && (status_closing == 1)) {

            printf("Server's queue is empty - work finished. closing down\n");
            exit(0);


        }

    }
        printf("Hello, World!\n");
        return 0;

}
