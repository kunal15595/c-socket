#ifndef _CLIENT_COMM_H_
#define _CLIENT_COMM_H_

typedef struct {
	int connfd;
	const char* file;
	const char* server_addr;
} client_comm;

client_comm *client_init(int connfd, const char* file, const char* server_addr);
void client_run(client_comm *cl);

#endif // _CLIENT_COMM_H_
