#include "client.hpp"
#include <cstring>
#include <stdexcept>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

Client::Client(int port){
    // create udp socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0){
        throw std::runtime_error("Socket creation failure");
    }
    // set broadcast option
    int ret;
    int yes = 1;
    ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&yes, sizeof(yes));
    if (ret == 1){
        throw std::runtime_error("Socket options setting failure");
    }
    //make sure broadcast address is zeroed-out
    memset((void*)&this->broadcast_addr, 0, sizeof(struct sockaddr_in));
    this->broadcast_addr.sin_family = AF_INET;
    this->broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    this->broadcast_addr.sin_port = htons(port);
}

void Client::send_msg(std::string msg){
    int ret = sendto(sock, 
            msg.data(), 
            msg.length(), 
            0, 
            (struct sockaddr*) &broadcast_addr, 
            sizeof(broadcast_addr));
    if (ret < 0){
        throw std::runtime_error("Message not sent");
    }
}
