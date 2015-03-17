#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "client_comm.h"
#include "string.h"


#define TCP_PORT 8787
#define QUEUE_SIZE 10
#define POOL_SIZE 4


void ctrl_c_signal_handler(int sig_num);

int main(int argc, char *argv[]){
    int tcp_port = 0;
    int sockid;
    if((sockid = socket(AF_INET,SOCK_STREAM,0))<0)
    {
        fprintf(stderr, "Error: Client Socket can not be created !! \n");
        fprintf(stderr, "errno : %d\n", errno);
        return -1;
    }

    char *server_addr = "127.0.0.1";
    char *file = "client.c";

    int opt;
    while ((opt = getopt (argc, argv, "p:i:f:")) != -1){
        switch(opt){
            case 'p':
                tcp_port = atoi(optarg);
                break;
            case 'i':
                server_addr = optarg;
                break;
            case 'f':
                file = optarg;
                break;
            case '?':
                //getopt gives ? if it is unable to recoganize an option
                fprintf(stderr, "Error: wrong format\n");
                printf("Usage ./client [-p tcp_port_server] [-i ip_addr_server] [-f file_to_transfer]\n");
                return -1;
                break;
        }
    }
    if(tcp_port == 0)
        tcp_port = TCP_PORT;

    printf("Client Sending TCP REQUEST on \n IP \t: %s\n PORT\t: %d\n", server_addr, tcp_port);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port=htons(tcp_port);
    if(inet_pton(AF_INET, server_addr, &serv_addr.sin_addr) <= 0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }
    if(connect(sockid, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    }
    printf("Server connected\n");
    client_comm *temp  = client_init(sockid,file,server_addr);
    client_run(temp);



    return 0;
}
