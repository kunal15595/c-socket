#include "client_comm.h"
#include "header.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>			//for htonl and sockaddr_in
#include <errno.h>				//for errno
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>				//for read,write,close

client_comm cl;

client_comm *client_init(int connfd, const char* file, const char* server_addr)
{
	cl.connfd = connfd;
	cl.file = file;
	cl.server_addr = server_addr;

	return &cl;
}

void client_run(client_comm *cl)
{
	char msg_buf[BUF_SIZE];
	struct message_header header;
	header.message_type = 1;
	header.message_length = 0;
	write(cl->connfd, &header, sizeof(header));

	struct message_header tcp_msg_header;

	if (read(cl->connfd, &tcp_msg_header, sizeof(tcp_msg_header)) == sizeof(tcp_msg_header)) {
		if (tcp_msg_header.message_type != 2) {
			printf("error in message type\n");
			close(cl->connfd);
			return;
		}
	} else {
		printf("error in getting the message header\n");
		close(cl->connfd);
		return;
	}
	int server_uport;

	if (read(cl->connfd, msg_buf, tcp_msg_header.message_length) == tcp_msg_header.message_length)
	{
		msg_buf[tcp_msg_header.message_length] = '\0';
		server_uport = atoi(msg_buf);

		printf("Server sent UDP PORT \t: %d\n", server_uport);
		close(cl->connfd); //tcp connection closed.
	}
	else
	{
		printf("%s\n", msg_buf);
		printf("error in getting the message\n");
		close(cl->connfd);
		return;
	}

	struct sockaddr_in client_udp;
	memset(&client_udp, '0', sizeof(client_udp));

	int udp_id = 0;
	if ((udp_id = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		fprintf(stderr, "Error : Could not create udp socket");
		fprintf(stderr, "Errno %d", errno);
		
		close(cl->connfd);
		return;
	} else
	{
		printf("UDP socket created : %d\n", udp_id);
	}

	client_udp.sin_family = AF_INET;
	client_udp.sin_addr.s_addr = htonl(INADDR_ANY);
	client_udp.sin_port = htons(0);

	if (bind(udp_id, (struct sockaddr *)&client_udp, sizeof(client_udp)) == -1) {

		fprintf(stderr, "Error : Binding with udp port failed");
		fprintf(stderr, "Errno %d", errno);

		if (errno == EADDRINUSE)
			fprintf(stderr, "Error : Another socket is already listening on the same port");
		close(cl->connfd);
		return;
	}

	FILE *fp = fopen(cl->file, "r");

	char buffer[BUF_SIZE];
	while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
		
		//fp.gcount() returns the number of characters read by the last read command on object.
		
		struct message_header header;
		header.message_type = 3;
		header.message_length = strlen(buffer);
		buffer[header.message_length] = '\0';
		char *message = buffer;

		socklen_t temp_len = sizeof(client_udp);

		if (inet_pton(AF_INET, cl->server_addr, &client_udp.sin_addr) <= 0)
		{
			printf("\n inet_pton error occured\n");
			return;
		}
		client_udp.sin_family = AF_INET;
		client_udp.sin_port = htons(server_uport);

		sendto(udp_id, &header, sizeof(header), MSG_WAITALL, (struct sockaddr *)&client_udp, temp_len);
		sendto(udp_id, message, strlen(message), MSG_WAITALL, (struct sockaddr *)&client_udp, temp_len);
		
		printf("Data Sent Over Udp Socket\n");

		struct message_header ack;

		if (recvfrom(udp_id, &ack, sizeof(ack), MSG_WAITALL, (struct sockaddr *)&client_udp, &temp_len) == sizeof(ack)){
			if (ack.message_type == 4)
				printf("Acknowledgement Recieved\n");
			else
				printf("Not expected message type from server but still continuing\n");
		}
		else{
			printf("Error has been occured but still continuing...\n");
		}
	}
	header.message_type = 3;
	header.message_length = 0;
	socklen_t temp_len = sizeof(client_udp);

	if (inet_pton(AF_INET, cl->server_addr, &client_udp.sin_addr) <= 0){
		printf("\n inet_pton error occured\n");
		return;
	}
	client_udp.sin_family = AF_INET;
	client_udp.sin_port = htons(server_uport);

	sendto(udp_id, &header, sizeof(header), MSG_WAITALL, (struct sockaddr *)&client_udp, temp_len);
	printf("Connection Terminating Packet sent\n");
	struct message_header ack;
	if (recvfrom(udp_id, &ack, sizeof(ack), MSG_WAITALL, (struct sockaddr *)&client_udp, &temp_len) == sizeof(ack)){
		if (ack.message_type == 4)
			printf("Acknowledgement Recieved\n");
		else
			printf("Not expected message type from server but still terminating ..\n");
	}
	else
	{
		printf("Error has been occured but still terminating...\n");
	}
	shutdown(udp_id, SHUT_RDWR);
	close(udp_id);
}
