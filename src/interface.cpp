#include "interface.hpp"
#include <ncurses.h>

Interface::Interface(Client* client){
    this->client = client;
    noecho();
    raw();
    cbreak();
    initscr();
    keypad(stdscr, TRUE);
    curs_set(0);
    chat.win = nullptr; textbox.win = nullptr; online_users.win = nullptr;
    if (has_colors()){
        //start_color();
    }
    fix_pane_sizes();
}

void Interface::update(){
    wnoutrefresh(chat.win);
    wnoutrefresh(textbox.win);
    wnoutrefresh(online_users.win);
    doupdate();
}

WINDOW* new_pane_window(Pane pane){
    WINDOW* win = newwin(pane.height, pane.width, pane.posy, pane.posx);
    box(win, 0, 0);
    wrefresh(win);
    return win;
}

void Interface::fix_pane_sizes(){
    if (chat.win != nullptr){
        free(chat.win);
    }
    if (textbox.win != nullptr){
        free(textbox.win);
    }
    if (online_users.win != nullptr){
        free(online_users.win);
    }
    // '3-quarters' layout
    chat.height = 3*(LINES/4);
    chat.width = 3*(COLS/4);
    chat.posx = 0;
    chat.posy = 0;
    textbox.height = LINES-chat.height;
    textbox.width = COLS;
    textbox.posx = 0;
    textbox.posy = chat.height;
    online_users.height = chat.height;
    online_users.width = COLS-chat.width;
    online_users.posx = chat.width;
    online_users.posy = 0;
    
    chat.win = new_pane_window(chat);
    textbox.win = new_pane_window(textbox);
    online_users.win = new_pane_window(online_users);

    draw_users_pane();
    draw_chat_pane();
}

void entitle_pane(Pane pane, const char* title){
    std::string titlestr = title;
    mvwaddstr(pane.win, 0, (pane.width-titlestr.length())/2, titlestr.data());
}

void Interface::draw_chat_pane(){
    int line = chat.height-2;
    entitle_pane(chat, "chat");
    for (int i=client->msgs.size()-1; i >= 1; i--){
        mvwaddstr(chat.win, line, 1,client->msgs[i].text.data());
        line--;
        if (line == 0) break;
    }
    wrefresh(chat.win);
}

void Interface::draw_users_pane(){
    int line = 1;
    entitle_pane(online_users, "online_users");
    for (int i=0; i < client->users.size(); i++){
        mvwaddstr(online_users.win, line, 1, (client->users[i].nickname).data());
        line++;
    }
    wrefresh(online_users.win);
}
