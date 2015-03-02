#include <iostream>
#include <arpa/inet.h>//for htonl and sockaddr_in
#include <unistd.h>//for getopt
#include <cstdlib>//for atoi
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>//for errno
#include "boost/threadpool.hpp"
#include <csignal>//for signal func


#define BUF_SIZE 1024
#define TCP_PORT 8787
#define QUEUE_SIZE 10
#define POOL_SIZE 4

using namespace std;
using namespace boost::threadpool;


void ctrl_c_signal_handler(int sig_num);
bool stop;

class send_udp_port{
    private:
        int connfd;
    public:
        send_udp_port(int connfd){
            this->connfd=connfd;
        }
        void run(){
            cout<<this->connfd<<endl; 
        }
};

int main(int argc, char *argv[]){
    int listenfd=0,connfd=0,tcp_port=0;

    struct sockaddr_in serv_addr;
    char buf[BUF_SIZE];
    char opt;

    //AF_INET connection with differnent machine
    //SOCK_STREAM TCP connection
    //SOCK_NONBLOCK mark socket as non blocking
    //when accept is called and there is no pending connections
    //accept blocks but with SOCK_NONBLOCK it returns immidiately with 
    //error EAGAIN or EWOULDBLOCK
    if((listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0))== -1)
    {
        cerr<<"Error : Could not create socket"<<endl;
        cerr<<"Errno "<<errno<<endl;
        return -1;
    }



    //AF_INET tell that the connection is with different machine
    //AF_UNIX connect inside same machine
    serv_addr.sin_family = AF_INET;

    //To accept connctions from all IPs
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    //optstring(last argument in getopt) is a string containing the legitimate option characters.
    //If such a character is followed by a colon, the option requires an
    //argument, so getopt() places a pointer to the following text in the
    //same argv-element, or the text of the following argv-element, in
    //optarg.  Two colons mean an option takes an optional arg
    while ((opt = getopt (argc, argv, "t:")) != -1){
        switch(opt){
            case 't':
                tcp_port=atoi(optarg);
                break;
            case '?':
                //getopt gives ? if it is unable to recoganize an option
                cerr<<"Error:wrong format"<<endl;
                cout<<"Usage ./server [-t tcp_port]"<<endl;
                return -1;
                break;
        }
    }

    if(tcp_port==0)
        tcp_port=8787;

    serv_addr.sin_port=htonl(tcp_port);

    if(bind(listenfd,(const struct sockaddr *)&serv_addr,sizeof(serv_addr))==-1){
        cerr<<"Error:Binding with port "<<tcp_port<<" failed"<<endl;
        cerr<<"Errno "<<errno<<endl;
        return -1;
    }


    //The backlog(second arg of listen) argument defines the maximum length to which the queue of
    //pending connections for sockfd may grow. If a connection request 
    //arrives when the queue is full, the client may receive an error
    // with an indication of ECONNREFUSED or, if the
    // underlying protocol supports retransmission, 
    //the request may be ignored so that a later reattempt at connection succeeds.
    if(listen(listenfd, QUEUE_SIZE) == -1){
        cerr<<"Error:Failed to listen"<<endl;
        cerr<<"Errno "<<errno<<endl;
        if(errno == EADDRINUSE)
            cerr<<"Another socket is already listening on the same port"<<endl;
        return -1;
    }else{
        cout<<"here"<<endl;
    }

    cout<<"Lintning on TCP port "<<tcp_port<<endl;

    pool tp(POOL_SIZE);

    signal(SIGINT,ctrl_c_signal_handler);
    stop=false;
    
    while(!stop){
        connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL); // accept awaiting request
        send_udp_port *temp=new send_udp_port(connfd);
        boost::threadpool::schedule(tp,boost::shared_ptr<send_udp_port>(temp));
        sleep(1);
    }
    
    return 0;
}

void ctrl_c_signal_handler(int sig_num){
    cout<<"Exiting"<<endl;
    stop=true;
}


