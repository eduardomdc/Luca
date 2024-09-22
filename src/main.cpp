#include <iostream>
#include <unistd.h>
#include <csignal>
#include <ncurses.h>
#include "client.hpp"
#include "interface.hpp"


Client* client = nullptr;

void handle_sigint(int signal){
    return; // only exit with F1
}

int main(int argc, char** argv){
    try{
        Interface interface = Interface();
        std::signal(SIGINT, handle_sigint);
        
        while (interface.is_running){
            interface.handle_input();
            interface.render();
        }
        endwin();
        interface.exit();
    } catch (std::exception& e){
        std::cerr << e.what();
        perror(" :");
    }
    return 0;
}
