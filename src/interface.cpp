#include "interface.hpp"
#include <ncurses.h>
#include <string>
#include <stdlib.h>

Interface::Interface(Client* client){
    this->client = client;
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(10);
    chat.win = nullptr; textbox.win = nullptr; online_users.win = nullptr;
    if (has_colors()){
        //start_color();
    }
    fix_pane_sizes();
}

void Interface::render(){
    draw_users_pane();
    draw_chat_pane();
    draw_textbox_pane();
}

void Interface::handle_input(){
    int ch = getch();
    if (ch >= 32 && ch <= 126){
        type(ch);
    } else if (ch == '\n'){
        send_typed();
    }
}

void Interface::type(int ch){
    std::string new_ch(1, char(ch));
    typed.append(new_ch);
}

void Interface::send_typed(){
    client->send_msg(typed);
    typed.clear();
    //draw_chat_pane();
}

WINDOW* new_pane_window(Pane pane){
    WINDOW* win = newwin(pane.height, pane.width, pane.posy, pane.posx);
    return win;
}

void Interface::fix_pane_sizes(){
    if (chat.win != nullptr){
        delwin(chat.win);
    }
    if (textbox.win != nullptr){
        delwin(textbox.win);
    }
    if (online_users.win != nullptr){
        delwin(online_users.win);
    }
    // '3-quarters' layout
    chat.height = 4*(LINES/5);
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
}

void entitle_pane(Pane pane, const char* title){
    std::string titlestr = title;
    mvwaddstr(pane.win, 0, (pane.width-titlestr.length())/2, titlestr.data());
}

void Interface::draw_chat_pane(){
    werase(chat.win);
    int line = chat.height-2;
    for (int i=client->msgs.size()-1; i >= 0; i--){
        mvwaddstr(chat.win, line, 1,client->msgs[i].text.data());
        line--;
        if (line == 0) break;
    }
    box(chat.win, 0, 0);
    entitle_pane(chat, "chat");
    wrefresh(chat.win);
}

void Interface::draw_textbox_pane(){
    werase(textbox.win);
    mvwaddstr(textbox.win, 1, 1, typed.data());
    box(textbox.win, 0, 0);
    wrefresh(textbox.win);
}

void Interface::draw_users_pane(){
    werase(online_users.win);
    int line = 1;
    for (int i=0; i < client->users.size(); i++){
        mvwaddstr(online_users.win, line, 1, (client->users[i].nickname).data());
        line++;
    }
    box(online_users.win, 0, 0);
    entitle_pane(online_users, "online users");
    wrefresh(online_users.win);
}
