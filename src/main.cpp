#include <iostream>
#include <thread>
#include <unistd.h>
#include <csignal>
#include <ncurses.h>
#include "client.hpp"

#define PORT 8000

Client client = Client(PORT, "Piccard", STANDARD);

void handle_sigint(int signal){
    client.farewell();
    endwin();
    std::exit(signal);
}

int main(){
    try{
        initscr();
        refresh();
        std::signal(SIGINT, handle_sigint);
        std::thread twait(&Client::wait_for_msgs, &client);
        while (true){
            sleep(2);
            client.send_msg("Tea. Earl Grey. Hot.");
            refresh();
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
