#ifndef INTERFACE
#define INTERFACE
#include "client.hpp"
#include <ncurses.h>

class Interface {
public:
    Interface(Client* client);
private:
    Client* client;
    WINDOW* chat_window;
    WINDOW* input_window;
    WINDOW* users_window;
};

#endif
