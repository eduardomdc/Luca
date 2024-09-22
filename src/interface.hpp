#ifndef INTERFACE
#define INTERFACE
#include "client.hpp"
#include <ncurses.h>
#include <string>
#include <thread>

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
    void draw_login_screen();
    void handle_input();
    void setup_colors();
    void leave_login();
    void exit();
    Client* client;
    bool is_running;
    bool login_screen;
    bool nickname_chosen;
    Color color_chosen;
private:
    Pane chat;
    Pane textbox;
    Pane online_users;
    std::string nickname;
    void type(int ch);
    void send_typed();
    void backspace_typed();
    std::string typed;
    std::thread twait;
};

#endif
