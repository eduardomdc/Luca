#ifndef INTERFACE
#define INTERFACE
#include "client.hpp"
#include <ncurses.h>

struct Pane {
    WINDOW* win;
    int width;
    int height;
    int posx;
    int posy;
}typedef Pane;

class Interface {
public:
    Interface(Client* client);
    void update();
    void fix_pane_sizes();
    void draw_chat_pane();
    void draw_users_pane();
private:
    Client* client;
    Pane chat;
    Pane textbox;
    Pane online_users;
};

#endif
