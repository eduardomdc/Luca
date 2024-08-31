#include "client.hpp"
#include <cstring>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

Client::Client(int port, Chat* chat){
    this->chat = chat;
    // create udp socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0){
        throw std::runtime_error("Socket creation failure");
    }
    int ret;
    int yes = 1;
    // Allow reuse of the address
    ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes));
    if (ret < 0) {
        throw std::runtime_error("Socket options setting failure REUSEADDR");
    }
    // set broadcast option
    ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&yes, sizeof(yes));
    if (ret < 0){
        throw std::runtime_error("Socket options setting failure BROADCAST");
    }
    
    //init local address
    memset((void*)&this->local_addr, 0, sizeof(struct sockaddr_in));
    this->local_addr.sin_family = AF_INET;
    this->local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    this->local_addr.sin_port = htons(port);
    //init broadcast address
    memset((void*)&this->broadcast_addr, 0, sizeof(struct sockaddr_in));
    this->broadcast_addr.sin_family = AF_INET;
    this->broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    this->broadcast_addr.sin_port = htons(port);
    
    // bind socket to local address
    ret = bind(sock, (struct sockaddr*)&local_addr, sizeof(struct sockaddr_in));
    if (ret < 0){
        throw std::runtime_error("Socket binding failure");
    }
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

void Client::wait_for_msgs(){
    // this thread will keep still to wait for msgs
    char buffer[1024];
    memset((void*) buffer, 0, sizeof(buffer));
    struct sockaddr_in sender_addr;
    socklen_t sender_addr_len;
    struct pollfd fds[1];
    fds[0].fd = sock;
    fds[0].events = POLLIN;

    while (true){
        int ret = poll(fds, 1, -1);
        memset((void*) buffer, 0, sizeof(buffer));
        if (ret > 0){
            if (fds[0].revents & POLLIN){
                recvfrom(sock, buffer, 1024, 0, (struct sockaddr*)&sender_addr, &sender_addr_len);
                printf("(%s): %s\n", inet_ntoa(sender_addr.sin_addr), buffer);
            }
        }
    }
}
