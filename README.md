# c-socket
Socket implementation in C

* Compile	
Server	--> gcc server.c server_comm.c threadpool.c -o server -pthread
Client	--> gcc client.c client_comm.c -o client

* Run
	* server usage --> ./server [-p tcp_port] [-t thread_pool_size]
	* client usage --> ./client [-p server_tcp_port] [-i server_ip] [-f file_to_transfer]

* You may get Adress already in use error if you run server program twice one after the other give a gap of 30 secs

TCP/IP stack keeps port busy for sometime even after close() so that the next  program that binds that port do not pick up packets intended for the original program
