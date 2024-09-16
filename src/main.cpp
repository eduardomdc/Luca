#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <csignal>
#include <ncurses.h>
#include "client.hpp"
#include "interface.hpp"

#define PORT 8000


Client* client = nullptr;

void handle_sigint(int signal){
    return; // only exit with F1
}

int main(int argc, char** argv){
    try{
        if (argc < 2 || argc > 3){
            printf("Usage: %s <nickname>\n", argv[0]);
            return -1;
        }
        char* nickname = argv[1];
        srand(time(NULL));
        Client client = Client(PORT, nickname, Color(rand()%WHITE+1));
        Interface interface = Interface();
        interface.client = &client;
        client.interface = &interface;
        std::signal(SIGINT, handle_sigint);
        std::thread twait(&Client::wait_for_msgs, &client);
        while (interface.is_running){
            interface.handle_input();
            interface.render();
        }
        endwin();
        twait.detach();
        client.farewell();
    } catch (std::exception& e){
        std::cerr << e.what();
        perror(" :");
    }
    return 0;
}
