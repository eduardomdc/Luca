#include "client.hpp"
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
 #include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

#define BUFFERSIZE 1024

Client::Client(int port, std::string nickname, Color color){
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

    // save me value
    User me_t;
    me_t.addr = local_addr.sin_addr;
    me_t.color = color;
    me_t.nickname = nickname;
    this->me = me_t;
    this->users.push_back(this->me);
}

void Client::send_msg(std::string msg){
    char* msg_data = msg.data();
    // serialize header and msg to buffer
    size_t buf_size = msg.size()+sizeof(Header);
    unsigned char* _buf = (unsigned char*) malloc(buf_size);
    memset(_buf, MSG, sizeof(Header));
    memcpy(&_buf[sizeof(Header)], msg_data, msg.size());

    int ret = sendto(sock, 
            _buf,
            buf_size,
            0, 
            (struct sockaddr*) &broadcast_addr, 
            sizeof(broadcast_addr));
    if (ret < 0){
        throw std::runtime_error("Message not sent");
    }
    free(_buf);
}

void Client::greet(){
    /* Greetings packages follow the format: 
        /1b   /1b   /x bytes
        /GREET/COLOR/NICKNAME
    */
    size_t buf_size = sizeof(Header)+sizeof(Color)+me.nickname.size();
    unsigned char* _buf = (unsigned char*) malloc(buf_size);
    
    free(_buf);
}

User Client::lookup_user(in_addr addr){
    User user;
    for (int i=0; i < this->users.size(); i++){
        long it = users[i].addr.s_addr;
        if (addr.s_addr == it){
            return users[i];
        }
    }
    return {{addr}, "Unknown User", RED};
}

void Client::read_msg(char buffer[], in_addr addr){
    User usr = this->lookup_user(addr);
    switch (buffer[0]){
        case MSG:
            printf("%s: %s\n", usr.nickname.data(), buffer+sizeof(Header));
            break;
        case GREET:
            break;
        case WELCOME:
            break;
        case FAREWELL:
            break;
        default:
            // msg not understood, ignore it.
            break;
    }
}

void Client::wait_for_msgs(){
    // this thread will keep still to wait for msgs
    char buffer[BUFFERSIZE];
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
                recvfrom(sock, buffer, BUFFERSIZE, 0, (struct sockaddr*)&sender_addr, &sender_addr_len);
                this->read_msg(buffer, sender_addr.sin_addr);
            }
        }
    }
}