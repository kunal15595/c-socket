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
    struct message_header req_header;
    req_header.message_type=1;
    req_header.message_length=0;
    write(sockfd,&req_header,sizeof(req_header));
    while ( (count = read(sockfd, message_buf, sizeof(struct message_header))) > 0)
    {
        message_buf[count] = '\0';
        memcpy(&input_header,message_buf,count);       
        cout<<input_header.message_type<<endl;
        cout<<input_header.message_length<<endl;
        if((count=read(sockfd,message_buf,input_header.message_length))==input_header.message_length){
            message_buf[count]='\0';
            int udp_port=atoi(message_buf);
            cout<<udp_port<<endl;
            struct sockaddr_in udp_socket;
            memset(&udp_socket,0,sizeof(udp_socket));
            int udp_fd=0;
            udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
            if (udp_fd ==-1)
            {
                fprintf(stderr, "Could not create socket.\n");
                exit(-1);
            }   
            if(inet_pton(AF_INET, argv[1], &udp_socket.sin_addr)<=0)
            {
                printf("\n inet_pton error occured\n");
                return 1;
            } 
            udp_socket.sin_family=AF_INET;
            udp_socket.sin_port = htons(udp_port); 

            struct message_header header;
            header.message_type=3;
            header.message_length=10;
            string temp1="shubhamers";
            socklen_t temp_len=sizeof(udp_socket);
            sendto(udp_fd,&header,sizeof(header),MSG_WAITALL,(struct sockaddr *)&udp_socket,temp_len);
            sendto(udp_fd,temp1.c_str(),temp1.size(),MSG_WAITALL,(struct sockaddr *)&udp_socket,temp_len);
            //You would think that close() would unblock the recvfrom(), but it doesn't on linux.
            struct message_header ack_recv_header;
            if((count=recvfrom(udp_fd,&ack_recv_header,sizeof(ack_recv_header),MSG_WAITALL,
                            (struct sockaddr *)&udp_socket,&temp_len))==sizeof(ack_recv_header)){
                if(ack_recv_header.message_type==4){
                    cout<<"ACK recieved"<<endl;
                }
            }

            shutdown(udp_fd, SHUT_RDWR);
            close(udp_fd);
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
