#include "interface.hpp"
#include "client.hpp"
#include <ncurses.h>
#include <string>
#include <stdlib.h>

#define PORT 8000

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
    login_screen = true;
    nickname_chosen = false;
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
    if (login_screen){
        draw_login_screen();
    } else {
        draw_users_pane();
        draw_chat_pane();
        draw_textbox_pane();
    }
}

void Interface::exit(){
    twait.detach();
    client->farewell();
}

void Interface::leave_login(){
    login_screen = false;
    client = new Client(PORT, nickname, color_chosen);
    twait = std::thread(&Client::wait_for_msgs, client);
}

void Interface::handle_input(){

    // user inputs his nickname
    if (login_screen && !nickname_chosen){
        int ch = getch();
        if (ch >= 32 && ch <= 126){
            type(ch);
        } else if (ch == '\n' && typed.size() > 0){
            nickname = typed;
            nickname_chosen = true;
            typed.clear();
        } else if (ch == KEY_BACKSPACE){
            backspace_typed();
        } else if (ch == KEY_END){
            is_running = false;
        }
    }

    // user chooses colour
    if (login_screen && nickname_chosen){
        int ch = getch();
        switch (ch) {
            case 'r':
                color_chosen = RED;
                leave_login();
                break;
            case 'g':
                color_chosen = GREEN;
                leave_login();
                break;
            case 'y':
                color_chosen = YELLOW;
                leave_login();
                break;
            case 'b':
                color_chosen = BLUE;
                leave_login();
                break;
            case 'm':
                color_chosen = MAGENTA;
                leave_login();
                break;
            case 'c':
                color_chosen = CYAN;
                leave_login();
                break;
            case 'w':
                color_chosen = WHITE;
                leave_login();
                break;
        }
        if (ch == KEY_END){
            is_running = false;
        }
    }

    // chat input handling
    if (!login_screen){
        int ch = getch();
        if (ch >= 32 && ch <= 126){
            type(ch);
        } else if (ch == '\n' && typed.size() > 0){
            send_typed();
        } else if (ch == KEY_BACKSPACE){
            backspace_typed();
        } else if (ch == KEY_END){
            is_running = false;
        }
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

const char* title[8] = {
    
" ___                               ",
"/\\_ \\                              ",
"\\//\\ \\    __  __    ___     __     ",
  "  \\ \\ \\  /\\ \\/\\ \\  /'___\\ /'__`\\   ",
   "   \\_\\ \\_\\ \\ \\_\\ \\/\\ \\__//\\ \\L\\.\\_ ",
   "   /\\____\\\\ \\____/\\ \\____\\ \\__/.\\_\\",
   "   \\/____/ \\/___/  \\/____/\\/__/\\/_/",
        "v0.2   Local UDP Chat Alternative",
};

void Interface::draw_login_screen(){
    erase();
    // draw title
    int title_size = sizeof(title)/sizeof(title[0]);
    for (int i=0; i<title_size; i++){
        mvaddstr(i, 2, title[i]);
    }

    if (!nickname_chosen){
        mvaddstr(title_size+1, 2, "Nickname: ");
        attron(COLOR_PAIR(TOOLTIP));
        mvaddstr(title_size+1, 12, typed.data());
        attroff(COLOR_PAIR(TOOLTIP));
    } else {
        mvaddstr(title_size+1, 2, "Choose your color");
        attron(COLOR_PAIR(RED));
        mvaddstr(title_size+2, 2, "r Red ");
        attroff(COLOR_PAIR(RED));
        attron(COLOR_PAIR(GREEN));
        addstr("g Green ");
        attroff(COLOR_PAIR(GREEN));
        attron(COLOR_PAIR(YELLOW));
        addstr("y Yellow ");
        attroff(COLOR_PAIR(YELLOW));
        attron(COLOR_PAIR(BLUE));
        addstr("b Blue ");
        attroff(COLOR_PAIR(BLUE));
        attron(COLOR_PAIR(MAGENTA));
        mvaddstr(title_size+3, 2, "m Magenta ");
        attroff(COLOR_PAIR(MAGENTA));
        attron(COLOR_PAIR(CYAN));
        addstr("c Cyan ");
        attroff(COLOR_PAIR(CYAN));
        attron(COLOR_PAIR(WHITE));
        addstr("w White ");
        attroff(COLOR_PAIR(WHITE));
    }
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
    mvwaddstr(chat.win, 0, 1, "END to quit");
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
    entitle_pane(online_users, "online");
    wrefresh(online_users.win);
}
