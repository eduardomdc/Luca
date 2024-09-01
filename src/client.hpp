#ifndef CLIENT
#define CLIENT
#include <netinet/in.h>
#include <string>
#include <vector>

enum Color : char {
    STANDARD,
    WHITE,
    RED,
    ORANGE
};

enum Header : char {
    MSG=1,
    GREET,
    WELCOME,
    FAREWELL,
};

struct User {
    in_addr addr;
    std::string nickname;
    Color color;
}typedef User;

struct Msg {
    User* author;
    std::string text;
    timespec time;
}typedef Msg;

class Client {
public:
    Client(int port, std::string Nickname, Color color);
    void send_msg(std::string msg);
    void read_msg(char data[], in_addr addr);
    void wait_for_msgs();
    void greet(); // user joins chat
    void welcome(); // user sends his username/ip
    void farewell(); // user leaves
    User lookup_user(in_addr addr);
private:
    int sock;
    struct sockaddr_in broadcast_addr;
    struct sockaddr_in local_addr;
    User me;
    std::vector<User> users;
    std::vector<Msg> msgs;
};

#endif
