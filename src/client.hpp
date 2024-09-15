#ifndef CLIENT
#define CLIENT
#include <chrono>
#include <netinet/in.h>
#include <string>
#include <vector>

class Interface;

enum Color : signed char {
    BLACK=1, 
    RED,
    GREEN,
    YELLOW, 
    BLUE, 
    MAGENTA,
    CYAN,
    WHITE,
    STANDARD,
    TOOLTIP
};

enum Header : char {
    FIND,
    MSG,
    GREET,
    WELCOME,
    FAREWELL,
};

struct User {
    in_addr addr;
    std::string nickname;
    Color color;
}typedef User;

struct Lookup {
    bool found;
    User user;
} typedef Lookup;

struct Msg {
    User author;
    std::string text;
    std::chrono::time_point<std::chrono::system_clock> time;
}typedef Msg;

class Client {
public:
    Client(int port, std::string nickname, Color color);
    void send_msg(std::string msg);
    void read_msg(char data[], in_addr addr);
    in_addr find_my_ip(); // find your own ip
    void wait_for_msgs();
    void greet(); // user joins chat
    void welcome(); // user replies greet
    void farewell(); // user leaves
    Lookup lookup_user(in_addr addr);
    User add_user(char greet[], in_addr addr);
    void remove_user(in_addr addr);
    User me;
    std::vector<User> users;
    std::vector<Msg> msgs;
    Interface* interface;
private:
    int sock;
    struct sockaddr_in broadcast_addr;
    struct sockaddr_in local_addr;
};

#endif
