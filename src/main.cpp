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
    if (client != nullptr){
        client->farewell();
    }
    endwin();
    std::exit(signal);
}

int main(){
    try{
        Client client = Client(PORT, "Scorpion", YELLOW);
        Interface interface = Interface();
        interface.client = &client;
        client.interface = &interface;
        std::signal(SIGINT, handle_sigint);
        std::thread twait(&Client::wait_for_msgs, &client);
        int i = 0;
        while (true){
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
