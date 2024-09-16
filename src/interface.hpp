#ifndef INTERFACE
#define INTERFACE
#include "client.hpp"
#include <ncurses.h>
#include <string>

struct Pane {
    WINDOW* win; // window with borders
    WINDOW* textwin; // text subwindow
    int width;
    int height;
    int posx;
    int posy;
}typedef Pane;

class Interface {
public:
    Interface();
    void render();
    void fix_pane_sizes();
    void draw_chat_pane();
    void draw_users_pane();
    void draw_textbox_pane();
    void handle_input();
    void setup_colors();
    Client* client;
    bool is_running;
private:
    Pane chat;
    Pane textbox;
    Pane online_users;
    void type(int ch);
    void send_typed();
    void backspace_typed();
    std::string typed;
};

#endif
