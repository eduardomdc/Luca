#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <csignal>
#include <ncurses.h>
#include "client.hpp"
#include "interface.hpp"

#define PORT 8000

Client client = Client(PORT, "Piccard", STANDARD);

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
            sleep(1);
            client.send_msg("I want "+std::to_string(i)+" tea. Earl Grey. Hot.");
            i++;
            //interface.update();
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
