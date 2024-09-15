#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <csignal>
#include <ncurses.h>
#include "client.hpp"
#include "interface.hpp"

#define PORT 8000

Client client = Client(PORT, "Ryker", GREEN);

void handle_sigint(int signal){
    client.farewell();
    endwin();
    std::exit(signal);
}

int main(){
    try{
        Interface interface = Interface(&client);
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
