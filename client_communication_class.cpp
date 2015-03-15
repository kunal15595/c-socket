#include "client_communication_class.h"
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

client_communication_class::client_communication_class(int connfd,const char* file,const char* server_addr)
{
    this->connfd = connfd;
    this->file = file;
    this->server_addr = server_addr;
}

void client_communication_class::run()
{
    char message_buf[BUF_SIZE];
    struct message_header header;
    header.message_type = 1;
    header.message_length = 0;
    write(this->connfd,&header,sizeof(header));

    struct message_header tcp_message_header;

    if(read(this->connfd,&tcp_message_header,sizeof(tcp_message_header))==sizeof(tcp_message_header)){
        if(tcp_message_header.message_type!=2){
            cout<<"error in message type"<<endl;
            close(this->connfd);
            return;
        }
    }else{
        cout<<"error in getting the message header"<<endl;
        close(this->connfd);
        return;
    }
    int server_udp_port;

    if(read(this->connfd,message_buf,tcp_message_header.message_length)==tcp_message_header.message_length)
    {
        string port="";
        message_buf[tcp_message_header.message_length]='\0';
        port = message_buf;
        stringstream uport(port);
        uport >> server_udp_port;
        cout<<"Server sent UDP PORT \t: "<<server_udp_port<<endl;
        close(this->connfd); //tcp connection closed.
    }
    else
    {
        cout<<message_buf<<endl;
        cout<<"error in getting the message"<<endl;
        close(this->connfd);
        return;
    }

    struct sockaddr_in client_udp;
    memset(&client_udp,'0',sizeof(client_udp));
    int udp_id=0;
    if((udp_id = socket(AF_INET, SOCK_DGRAM, 0))== -1)
    {
        cerr<<"Error : Could not create udp socket"<<endl;
        cerr<<"Errno "<<errno<<endl;
        close(this->connfd);
        return;
    }else
    {
        cout<<"UDP socket created : "<<udp_id<<endl;
    }

    client_udp.sin_family = AF_INET;
    client_udp.sin_addr.s_addr=htonl(INADDR_ANY);
    client_udp.sin_port=htons(0);

    if(bind(udp_id,(struct sockaddr *)&client_udp,sizeof(client_udp))==-1){
        cerr<<"Error:Binding with udp port failed"<<endl;
        cerr<<"Errno "<<errno<<endl;
        if(errno == EADDRINUSE)
            cerr<<"Another socket is already listening on the same port"<<endl;
        close(this->connfd);
        return;
    }

    ifstream FILE(this->file,ios::in);
    char buffer[BUF_SIZE];
    while(!FILE.eof())
    {
        FILE.read(buffer,sizeof(buffer));
        //FILE.gcount() returns the number of characters read by the last read command on object.
        struct message_header header;
        header.message_type = 3;
        header.message_length = FILE.gcount();
        buffer[header.message_length] = '\0';
        string message = buffer;

        socklen_t temp_len=sizeof(client_udp);

        if(inet_pton(AF_INET, server_addr, &client_udp.sin_addr)<=0)
        {
            printf("\n inet_pton error occured\n");
            return;
        }
        client_udp.sin_family=AF_INET;
        client_udp.sin_port = htons(server_udp_port);

        sendto(udp_id,&header,sizeof(header),MSG_WAITALL,(struct sockaddr *)&client_udp,temp_len);
        sendto(udp_id,message.c_str(),message.size(),MSG_WAITALL,(struct sockaddr *)&client_udp,temp_len);
        cout<<"Data Sent Over Udp Socket"<<endl;

        struct message_header ack;

        if(recvfrom(udp_id,&ack,sizeof(ack),MSG_WAITALL,(struct sockaddr *)&client_udp,&temp_len)==sizeof(ack))
        {
            if(ack.message_type == 4)
            cout<<"Acknowledgement Recieved"<<endl;
            else
            cout<<"Not expected message type from server but still continuing"<<endl;
        }
        else
        {
            cout<<"Error has been occured but still continuing..."<<endl;
        }
    }
    shutdown(udp_id, SHUT_RDWR);
    close(udp_id);
}
