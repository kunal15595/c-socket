#include "server_comm.h"
#include <stdio.h>
#include <arpa/inet.h>//for htonl and sockaddr_in
#include "header.h"
#include <errno.h>//for errno
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>//for atoi
#include <unistd.h>//for read,write,close
#include <string.h>//for memset and memcpy
#include <stdbool.h>


void server_run(int connfd){
    struct message_header tcp_msg_head;
    if(read(connfd,&tcp_msg_head,sizeof(tcp_msg_head))==sizeof(tcp_msg_head)){
        if(tcp_msg_head.message_type!=1){
            close(connfd);
            return;
        }
    }else{
        close(connfd);
        return;
    }

    struct sockaddr_in serv_addr_udp;
    memset(&serv_addr_udp,'0',sizeof(serv_addr_udp));
    serv_addr_udp.sin_family = AF_INET;
    serv_addr_udp.sin_addr.s_addr=htonl(INADDR_ANY);

    //0 means that it will choose a random port
    serv_addr_udp.sin_port=htons(0);

    int udp_fd=0;
    //Not SOCK_NONBLOCK
    if((udp_fd = socket(AF_INET, SOCK_DGRAM, 0))== -1){
        printf("Error : Could not create udp socket\n");
        printf("Errno %d\n",errno);
        close(connfd);
        return;
    }

    if(bind(udp_fd,(struct sockaddr *)&serv_addr_udp,sizeof(serv_addr_udp))==-1){
        printf("Error:Binding with udp port failed\n");
        printf("Errno %d\n",errno);
        if(errno == EADDRINUSE)
            printf("Another socket is already listening on the same port\n");
        close(connfd);
        return;
    }

    //No listen in case of udp socket

    //To get port number assigned to socket
    memset(&serv_addr_udp,'0',sizeof(serv_addr_udp));
    socklen_t len=(socklen_t)sizeof(serv_addr_udp);
    getsockname(udp_fd,(struct sockaddr *)&serv_addr_udp,&len);

    int udp_port=ntohs(serv_addr_udp.sin_port);

    printf("UDP port %d \n",udp_port); 

    char message_buf[BUF_SIZE];
    struct message_header header;

    header.message_type=2;

    sprintf(message_buf,"%d%c",udp_port,'\0');
    header.message_length=(int)strlen(message_buf);

    write(connfd,&header,sizeof(header));
    write(connfd,message_buf,strlen(message_buf));

    // close tcp connection
    close(connfd);

    int count;
    struct message_header input_header;
    struct message_header ack_header;
    struct sockaddr from_socket_addr;

    socklen_t temp_len;
    temp_len=sizeof(from_socket_addr);

    //recvfrom flag MSG_WAITALL
    //This flag requests that the operation block until the full request is satisfied
    
    bool done=false;

    while(!done){
        if((count=recvfrom(udp_fd,message_buf,sizeof(struct message_header),MSG_WAITALL,
                    &from_socket_addr,&temp_len))==sizeof(struct message_header)){
            memcpy(&input_header,message_buf,count);       
            if(input_header.message_length==0){
                done=true;
                printf("Ending received \n");
                ack_header.message_type=4;
                ack_header.message_length=0;
                sendto(udp_fd,&ack_header,sizeof(ack_header),MSG_WAITALL,(struct sockaddr *)&from_socket_addr,temp_len);
                
            }else if((count=recvfrom(udp_fd,message_buf,input_header.message_length,
                            MSG_WAITALL,&from_socket_addr,&temp_len))==input_header.message_length){
                message_buf[count]='\0';
                printf("%s\n",(char *)message_buf);

                ack_header.message_type=4;
                ack_header.message_length=0;

                sendto(udp_fd,&ack_header,sizeof(ack_header),MSG_WAITALL,(struct sockaddr *)&from_socket_addr,temp_len);
            }else{
                printf("Error on port %d\n",udp_port);
                printf("Should have received %d bytes but received %d bytes\n",input_header.message_length,count);
                done=true;
            }


        }else{
            printf("Error on port %d\n",udp_port);
            done=true;
        }
    }
    close(udp_fd);
    printf("Connection over\n");
}

