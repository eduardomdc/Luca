#ifndef CHAT
#define CHAT

#include <ctime>
#include <vector>
#include <string>
#include <netinet/in.h>

enum Color {
    STANDARD,
    WHITE,
    RED,
    ORANGE
};

struct User {
    sockaddr_in addr;
    std::string nickname;
    Color color;
}typedef User;

struct Msg {
    User* author;
    std::string text;
    timespec time;
}typedef Msg;

class Chat {
public:
    Chat();
    void login();
    User me;
private:
    std::vector<User> users;
    std::vector<Msg> msgs;
};

#endif
