#include "client.hpp"
#include <cstddef>
#include <cstdlib>
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
#include <time.h>
#include <chrono>
#include <ncurses.h>
#include "interface.hpp"

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

    this->me.addr = find_my_ip();
    this->me.color = color;
    this->me.nickname = nickname;
    users.push_back(this->me);
    this->greet();
}

void Client::send_msg(std::string msg){
    char* msg_data = msg.data();
    // serialize header and msg to buffer
    size_t buf_size = msg.size()+sizeof(Header);
    unsigned char _buf[buf_size];
    memset(_buf, MSG, sizeof(Header));
    memcpy(_buf+sizeof(Header), msg_data, msg.size());

    int ret = sendto(sock, 
            _buf,
            buf_size,
            0, 
            (struct sockaddr*) &broadcast_addr, 
            sizeof(broadcast_addr));
    if (ret < 0){
        throw std::runtime_error("Message not sent");
    }
}

void Client::greet(){
    /* Greetings packages follow the format: 
        /1b   /1b   /x bytes
        /GREET/COLOR/NICKNAME */
    size_t buf_size = sizeof(Header)+sizeof(Color)+me.nickname.size();
    unsigned char _buf[buf_size];
    memset(_buf, GREET, sizeof(Header));
    memset(_buf+sizeof(Header), me.color, sizeof(Color));
    memcpy(_buf+sizeof(Header)+sizeof(Color), me.nickname.data(), me.nickname.size());
    int ret = sendto(sock, 
            _buf,
            buf_size,
            0, 
            (struct sockaddr*) &broadcast_addr, 
            sizeof(broadcast_addr));
    if (ret < 0){
        throw std::runtime_error("Message not sent");
    }
}

void Client::welcome(){
    // same as greet package but different header value
    size_t buf_size = sizeof(Header)+sizeof(Color)+me.nickname.size();
    unsigned char _buf[buf_size];
    memset(_buf, WELCOME, sizeof(Header));
    memset(_buf+sizeof(Header), me.color, sizeof(Color));
    memcpy(_buf+sizeof(Header)+sizeof(Color), me.nickname.data(), me.nickname.size());
    int ret = sendto(sock, 
            _buf,
            buf_size,
            0, 
            (struct sockaddr*) &broadcast_addr, 
            sizeof(broadcast_addr));
    if (ret < 0){
        throw std::runtime_error("Message not sent");
    }
}

void Client::farewell(){
    char bye[9] = "Goodbye!";
    size_t buf_size = sizeof(Header)+sizeof(bye);
    unsigned char _buf[buf_size];
    memset(_buf, FAREWELL, sizeof(Header));
    memcpy(_buf+sizeof(Header), bye, sizeof(bye));
    int ret = sendto(sock, 
            _buf,
            buf_size,
            0, 
            (struct sockaddr*) &broadcast_addr, 
            sizeof(broadcast_addr));
    if (ret < 0){
        throw std::runtime_error("Message not sent");
    }
}

Lookup Client::lookup_user(in_addr addr){
    Lookup lookup;
    for (unsigned int i=0; i < this->users.size(); i++){
        long it = users[i].addr.s_addr;
        if (addr.s_addr == it){
            return {true, users[i]};
        }
    }
    return {false, {{addr}, "Unknown User", RED}};
}

User Client::add_user(char greet[], in_addr addr){
    User usr;
    memcpy(&usr.color, greet+sizeof(Header), sizeof(Color));
    usr.nickname.assign(greet+sizeof(Header)+sizeof(Color));
    usr.addr = addr;
    this->users.push_back(usr);
    this->interface->draw_users_pane();
    return usr;
}

void Client::remove_user(in_addr addr){
    for (auto it = users.begin(); it != users.end();){
        if (it->addr.s_addr == addr.s_addr) {
            it = users.erase(it);
        } else {
            it++;
        }
    }
}

void Client::read_msg(char buffer[], in_addr addr){
    int yourself = 0;
    if (addr.s_addr == me.addr.s_addr) yourself = 1;
    Lookup lookup = this->lookup_user(addr);
    Msg msg;
    switch (buffer[0]){
        case FIND:
            // ignore find requests, see find_my_ip()
            break;
        case MSG:
            if (lookup.found){
                //printw("%s: %s\n", lookup.user.nickname.data(), buffer+sizeof(Header));
                msg.author = lookup.user;
                msg.text = lookup.user.nickname + ": " + (buffer+sizeof(Header));
                msg.time = std::chrono::system_clock::now();
                msgs.push_back(msg);
                //interface->draw_chat_pane();
                break;
            }
        case GREET:
            // reply with welcome, add to known users
            if (!yourself){
                this->welcome();
                if (!lookup.found){
                    lookup.user = this->add_user(buffer, addr);
                    //printf("%s joined the chat.\n", lookup.user.nickname.data());
                    msg.author = lookup.user;
                    msg.text = lookup.user.nickname + " joined the chat.";
                    msg.time = std::chrono::system_clock::now();
                    msgs.push_back(msg);
                    //interface->draw_chat_pane();
                }
            }
            break;
        case WELCOME:
            // add to known users
            if (!yourself){
                if (!lookup.found){
                    lookup.user = this->add_user(buffer, addr);
                    //interface->draw_users_pane();
                }
            }
            break;
        case FAREWELL:
            if (!yourself){
                if (lookup.found){
                    this->remove_user(addr);
                    msg.author = lookup.user;
                    msg.text = lookup.user.nickname + " left.";
                    msg.time = std::chrono::system_clock::now();
                    msgs.push_back(msg);
                    //interface->draw_chat_pane();
                }
            }
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
    socklen_t sender_addr_len = sizeof(sender_addr);
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

in_addr Client::find_my_ip(){
    // broadcast random int key and wait 
    // for reply with the key content match
    // then return key sender ip

    // generate random key
    srand(time(NULL));
    int key = rand();
    size_t key_size = sizeof(Header)+sizeof(key);
    char key_sent[key_size];
    memset(key_sent, FIND, sizeof(Header));
    memcpy(&key_sent[sizeof(Header)], &key, sizeof(key));
    
    char answer[key_size];
    memset((void*) answer, 0, sizeof(answer));

    struct sockaddr_in sender_addr;
    socklen_t sender_addr_len = sizeof(sender_addr);
    struct pollfd fds[1];
    fds[0].fd = sock;
    fds[0].events = POLLIN;

    int attempts = 0;
    const int max_attempts = 100;
    while (attempts < max_attempts){
        // send key
        int ret = sendto(sock, 
                key_sent,
                key_size,
                0, 
                (struct sockaddr*) &broadcast_addr, 
                sizeof(broadcast_addr));
        if (ret < 0){
            throw std::runtime_error("Message not sent");
        }
        // poll for answers
        ret = poll(fds, 1, 5000);
        memset((void*) answer, 0, sizeof(answer));
        if (ret > 0){
            if (fds[0].revents & POLLIN){
                recvfrom(sock, answer, key_size, 0, (struct sockaddr*)&sender_addr, &sender_addr_len);
                // check if answer is key
                if (memcmp(answer, key_sent, key_size) == 0){
                    // key found, return sender ip
                    return sender_addr.sin_addr;
                }
                // key not found, repeat
            }
        }
        attempts++;
    }
    throw std::runtime_error("Could not find my ip address");
}
