# c-socket
Socket implementation in C

* compiling server
  * Compiling command- gcc server.c server_communication_class.c threadpool.c -o server -pthread
* server usage ./server [-p tcp_port] [-t thread_pool_size]
* You may get Adress already in use error if you run server program twice one after the other give a gap of 30 secs
TCP/IP stack keeps port busy for sometime even after close() so that the next 
program that binds that port do not pick up packets intended for the original program
