#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<pthread.h>
void* doProcess(void*);
void* resource(void*);
void abc(int);
pthread_mutex_t a_mutex = PTHREAD_MUTEX_INITIALIZER;
int queue[100];
int front=-1;
int rear=-1;
int main(int count, char *args[])
{
int socket_file_descriptor, new_socket_file_descriptor;
int port_number;
int client_address_size;
// read or write
int bind_status;
// server  reads from socket connection into this buffer;
struct sockaddr_in server_address,client_address;
if(count<2)
{
printf("port number not provided");
return 0;
}
port_number=atoi(args[1]);
socket_file_descriptor=socket(AF_INET,SOCK_STREAM,0);
if(socket_file_descriptor<0)
{
printf("ERROR creating socket");
return 0;
}
// setting all fields of server address structure to zero;
//i.e setting values in buffer equals to zero
bzero((char*)&server_address,sizeof(server_address));
server_address.sin_family=AF_INET;
server_address.sin_port=htons(port_number);
// INADDR_ANY is symbolic for the IP address of the host which is running the server
server_address.sin_addr.s_addr=INADDR_ANY;
bind_status=bind(socket_file_descriptor,(struct sockaddr *)&server_address,sizeof(server_address));
if(bind_status<0)
{
printf("Error on binding");
return 0;
}
listen(socket_file_descriptor,5);
client_address_size=sizeof(client_address);
pthread_t thread1;
pthread_create(&thread1,NULL,resource,(void*)NULL);
while(1)
{
new_socket_file_descriptor=accept(socket_file_descriptor,(struct sockaddr*)&client_address,&client_address_size);
if(new_socket_file_descriptor<0)
{
printf("Error on accepting connection!");
return 0;
}
pthread_t thread2;
pthread_create(&thread2,NULL,doProcess,(void*)new_socket_file_descriptor);
}
return 0;
}
void* doProcess(void* sock)
{
int rc;
char read_buffer[256];
int number_of_characters;
bzero(read_buffer,256);
number_of_characters=read(sock,read_buffer,255);
if(number_of_characters<0)
{
printf("Error Reading from socket");
}
printf("message: %s\n",read_buffer);
// X denotes a particular type of resource , in our case calling abc;
if(read_buffer[0]=='X')
{
pthread_mutex_lock(&a_mutex);
rear++;
queue[rear]=sock;
pthread_mutex_unlock(&a_mutex);
}
else
{
number_of_characters=write(sock,"MESSAGE RECEIVED",16);
if(number_of_characters<0) printf("ERROR SENDING ACKNOWLEDGEMENT");
}
}


void* resource(void* args)
{
int rc;
while(1)
{
if(front<rear)
{
pthread_mutex_lock(&a_mutex);
front++;
abc(queue[front]);
pthread_mutex_unlock(&a_mutex);
}
}
}


void abc(int sock)
{
printf("Processing request for %d socketID",sock);
sleep(10);
int number_of_characters=write(sock,"MESSAGE RECEIVED AND REQUEST PROCESSED",38);
if(number_of_characters<0) printf("ERROR SENDING ACKNOWLEDGEMENT");
printf("Request processed\n");
}

