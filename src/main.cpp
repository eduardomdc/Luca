#include <iostream>
#include <thread>
#include <unistd.h>
#include <csignal>
#include "client.hpp"

#define PORT 8000

Client client = Client(PORT, "Piccard", STANDARD);

void handle_sigint(int signal){
    client.farewell();
    std::exit(signal);
}

int main(){
    try{
        std::signal(SIGINT, handle_sigint);
        std::thread twait(&Client::wait_for_msgs, &client);
        while (true){
            sleep(2);
            client.send_msg("Tea. Earl Grey. Hot.");
        }
        twait.detach();
        client.farewell();
    } catch (std::exception& e){
        std::cerr << e.what();
        perror(" :");
    }
    return 0;
}
