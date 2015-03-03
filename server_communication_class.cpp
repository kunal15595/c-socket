#include "server_communication_class.h"
#include <iostream>
#include <arpa/inet.h>//for htonl and sockaddr_in
#include "header.h"
#include <sstream>
#include <errno.h>//for errno
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>//for atoi
#include <unistd.h>//for read,write,close
#include <cstring>//for memset and memcpy


using namespace std;

server_communication_class::server_communication_class(int connfd){
    this->connfd=connfd;
}

void server_communication_class::run(){
    struct message_header tcp_message_header;
    if(read(this->connfd,&tcp_message_header,sizeof(tcp_message_header))==sizeof(tcp_message_header)){
        if(tcp_message_header.message_type!=1){
            close(this->connfd);
            return;
        }
    }else{
        close(this->connfd);
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
    if((udp_fd = socket(AF_INET, SOCK_DGRAM, 0))== -1)
    {
        cerr<<"Error : Could not create udp socket"<<endl;
        cerr<<"Errno "<<errno<<endl;
        close(this->connfd);
        return;
    }

    if(bind(udp_fd,(struct sockaddr *)&serv_addr_udp,sizeof(serv_addr_udp))==-1){
        cerr<<"Error:Binding with udp port failed"<<endl;
        cerr<<"Errno "<<errno<<endl;
        if(errno == EADDRINUSE)
            cerr<<"Another socket is already listening on the same port"<<endl;
        close(this->connfd);
        return;
    }

    //No listen in case of udp socket

    //To get port number assigned to socket
    memset(&serv_addr_udp,'0',sizeof(serv_addr_udp));
    socklen_t len=(socklen_t)sizeof(serv_addr_udp);
    getsockname(udp_fd,(struct sockaddr *)&serv_addr_udp,&len);

    int udp_port=ntohs(serv_addr_udp.sin_port);

    cout<<"UDP port "<<udp_port<<endl; 

    char message_buf[BUF_SIZE];
    struct message_header header;

    header.message_type=2;
    stringstream temp;
    temp<<udp_port;
    string temp_str=temp.str();
    header.message_length=temp_str.size();
    temp_str.copy(message_buf,temp_str.size(),0);

    write(this->connfd,&header,sizeof(header));
    write(this->connfd,message_buf,temp_str.size());

    close(this->connfd);

    int count;
    struct message_header input_header;
    struct message_header ack_header;
    struct sockaddr from_socket_addr;
    socklen_t temp_len;
    temp_len=sizeof(from_socket_addr);
    //recvfrom flag MSG_WAITALL
    //This flag requests that the operation block until the full request is satisfied

    if((count=recvfrom(udp_fd,message_buf,sizeof(struct message_header),MSG_WAITALL,
                    &from_socket_addr,&temp_len))==sizeof(struct message_header)){
        memcpy(&input_header,message_buf,count);       
        if((count=recvfrom(udp_fd,message_buf,input_header.message_length,
                        MSG_WAITALL,&from_socket_addr,&temp_len))==input_header.message_length){
            message_buf[count]='\0';
            cout<<(char *)message_buf<<endl;

            ack_header.message_type=4;
            ack_header.message_length=0;

            sendto(udp_fd,&ack_header,sizeof(ack_header),MSG_WAITALL,(struct sockaddr *)&from_socket_addr,temp_len);
        }
    }
    close(udp_fd);
    cout<<"Here"<<endl;

}

