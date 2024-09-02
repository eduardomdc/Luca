#include "interface.hpp"

Interface::Interface(Client* client){
    this->client = client;
    noecho();
    raw();
    initscr();
    keypad(stdscr, TRUE);
    refresh();
}
