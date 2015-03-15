#include<bits/stdc++.h>
#include<arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "client_communication_class.h"
#include "client_communication_class.cpp"
#include<unistd.h>

#define TCP_PORT 8787
#define QUEUE_SIZE 10
#define POOL_SIZE 4

using namespace std;

void ctrl_c_signal_handler(int sig_num);
bool stop;

int main(int argc, char *argv[]){
    int tcp_port = 0;
    int sockid;
    if((sockid = socket(AF_INET,SOCK_STREAM,0))<0)
    {
        cerr<<"Error: Client Socket can not be created !! "<<endl;
        cerr<<"errno : "<<errno<<endl;
        return -1;
    }

    string server_addr="127.0.0.1";
    string file = "client.cpp";

    int opt;
    while ((opt = getopt (argc, argv, "p:i:f:")) != -1){
        switch(opt){
            case 'p':
                tcp_port=atoi(optarg);
                break;
            case 'i':
                server_addr=optarg;
                break;
            case 'f':
                file = optarg;
                break;
            case '?':
                //getopt gives ? if it is unable to recoganize an option
                cerr<<"Error:wrong format"<<endl;
                cout<<"Usage ./client [-p tcp_port_server] [-i ip_addr_server] [-f file_to_transfer]"<<endl;
                return -1;
                break;
        }
    }
    if(tcp_port == 0)
        tcp_port = TCP_PORT;

    cout<<"Client Sending TCP REQUEST on \n IP \t: "<<server_addr<<"\n PORT\t: "<<tcp_port<<endl;

    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port=htons(tcp_port);
    if(inet_pton(AF_INET, server_addr.c_str(), &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }
    if(connect(sockid, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    }
    cout<<"Server connected"<<endl;
    client_communication_class *temp  = new client_communication_class(sockid,file.c_str(),server_addr.c_str());
    temp->run();



    return 0;
}
