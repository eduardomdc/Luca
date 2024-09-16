#include "interface.hpp"
#include "client.hpp"
#include <ncurses.h>
#include <string>
#include <stdlib.h>

Interface::Interface(){
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(10);
    chat.win = nullptr; textbox.win = nullptr; online_users.win = nullptr;
    if (has_colors()){
        start_color();
        use_default_colors();
        setup_colors();
    }
    fix_pane_sizes();
    is_running = true;
}

void Interface::setup_colors(){
    init_pair(STANDARD, STANDARD, STANDARD);
    init_pair(BLACK, COLOR_BLACK, -1);
    init_pair(RED, COLOR_RED, -1);
    init_pair(GREEN, COLOR_GREEN, -1);
    init_pair(YELLOW, COLOR_YELLOW, -1);
    init_pair(BLUE, COLOR_BLUE, -1);
    init_pair(MAGENTA, COLOR_MAGENTA, -1);
    init_pair(CYAN, COLOR_CYAN, -1);
    init_pair(WHITE, COLOR_WHITE, -1);
    init_pair(TOOLTIP, COLOR_BLACK, COLOR_WHITE);
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
    } else if (ch == '\n' && typed.size() > 0){
        send_typed();
    } else if (ch == KEY_BACKSPACE){
        backspace_typed();
    } else if (ch == KEY_F(1)){
        is_running = false;
    }
}

void Interface::type(int ch){
    std::string new_ch(1, char(ch));
    typed.append(new_ch);
}

void Interface::send_typed(){
    client->send_msg(typed);
    typed.clear();
}

void Interface::backspace_typed(){
    if (typed.length() > 0)
        typed.pop_back();
}

void new_pane_window(Pane* pane){
    pane->win = newwin(pane->height, pane->width, pane->posy, pane->posx);
    pane->textwin = derwin(pane->win, pane->height-2, pane->width-2, 1, 1); 
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
    
    new_pane_window(&chat);
    new_pane_window(&textbox);
    new_pane_window(&online_users);
}

void entitle_pane(Pane pane, const char* title){
    std::string titlestr = title;
    mvwaddstr(pane.win, 0, (pane.width-titlestr.length())/2, titlestr.data());
}

void Interface::draw_chat_pane(){
    werase(chat.win);
    int line = chat.height-2;
    for (int i=client->msgs.size()-1; i >= 0; i--){
        Msg this_msg = client->msgs[i];
        std::string msg = this_msg.text;
        int spacing = 1 + msg.length()/(chat.width-2);//space needed for msg
        wattron(chat.textwin,COLOR_PAIR(this_msg.author.color));
        mvwaddstr(chat.textwin, line-spacing, 0,msg.data());
        wattroff(chat.textwin,COLOR_PAIR(this_msg.author.color));
        line -= spacing;
        if (line-1 < 0) break;
    }
    box(chat.win, 0, 0);
    entitle_pane(chat, "|Round Table|");
    mvwaddstr(chat.win, 0, 1, "F1 to quit");
    wrefresh(chat.win);
}

void Interface::draw_textbox_pane(){
    werase(textbox.win);
    wattron(textbox.textwin, A_DIM);
    mvwaddstr(textbox.textwin, 0, 0, " Type your message");
    wattroff(textbox.textwin, A_DIM);
    mvwaddstr(textbox.textwin, 0, 0, typed.data());
    wattron(textbox.textwin, COLOR_PAIR(TOOLTIP));
    waddch(textbox.textwin, ' ');
    wattroff(textbox.textwin, COLOR_PAIR(TOOLTIP));
    box(textbox.win, 0, 0);
    wrefresh(textbox.win);
}

void Interface::draw_users_pane(){
    werase(online_users.win);
    int line = 0;
    for (unsigned int i=0; i < client->users.size(); i++){
        wattron(online_users.textwin,COLOR_PAIR(client->users[i].color));
        mvwaddstr(online_users.textwin, line, 0, (client->users[i].nickname).data());
        wattroff(online_users.textwin,COLOR_PAIR(client->users[i].color));
        line++;
    }
    box(online_users.win, 0, 0);
    entitle_pane(online_users, "online users");
    wrefresh(online_users.win);
}
