//Dummy client
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include "header.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char message_buf[1024];
    struct sockaddr_in serv_addr; 

    if(argc != 2)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    } 

    memset(message_buf, '0',sizeof(message_buf));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000); 

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 

    struct message_header input_header;
    int count;
    while ( (count = read(sockfd, message_buf, sizeof(struct message_header))) > 0)
    {
        message_buf[count] = '\0';
        memcpy(&input_header,message_buf,count);       
        cout<<input_header.message_type<<endl;
        cout<<input_header.message_length<<endl;
        if((count=read(sockfd,message_buf,input_header.message_length))==input_header.message_length){
            message_buf[count]='\0';
            cout<<message_buf<<endl;
        }else{
            cout<<"Hi"<<endl;
        }

    } 

    printf("\n");

    if(n < 0)
    {
        printf("\n Read error \n");
    } 

    return 0;
}
