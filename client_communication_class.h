#include <bits/stdc++.h>
#ifndef CLIENT_COMMUNICATION_CLASS_H_INCLUDED
#define CLIENT_COMMUNICATION_CLASS_H_INCLUDED
class client_communication_class{
    private:
        int connfd;
        const char* file;
        const char* server_addr;
    public:
        client_communication_class(int,const char* ,const char* );
        void run();
};
#endif // CLIENT_COMMUNICATION_CLASS_H_INCLUDED
