#ifndef SERVER_COMM_CLASS
#define SERVER_COMM_CLASS
#include <iostream>
class server_communication_class{
    private:
        int connfd;
    public:
        server_communication_class(int);
        void run();
};
#endif
