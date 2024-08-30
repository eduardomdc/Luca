#ifndef CLIENT
#define CLIENT
#include <arpa/inet.h>
#include <string>

class Client {
public:
    Client(int port);
    void send_msg(std::string msg);
private:
    int sock;
    struct sockaddr_in broadcast_addr;
};

#endif
