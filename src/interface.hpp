#ifndef INTERFACE
#define INTERFACE
#include "client.hpp"
#include <ncurses.h>
#include <string>

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
    void render();
    void fix_pane_sizes();
    void draw_chat_pane();
    void draw_users_pane();
    void draw_textbox_pane();
    void handle_input();
private:
    Client* client;
    Pane chat;
    Pane textbox;
    Pane online_users;
    void type(int ch);
    void send_typed();
    std::string typed;
};

#endif
