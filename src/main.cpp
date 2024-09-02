#include <iostream>
#include <thread>
#include <unistd.h>
#include "client.hpp"

#define PORT 8000

int main(){
    try{
        Client client = Client(PORT, "Piccard", STANDARD);
        std::thread twait(&Client::wait_for_msgs, &client);
        while (true){
            client.send_msg("Hello");
            sleep(1);
            client.send_msg("Hello2");
            sleep(1);
        }
        twait.detach();
        client.farewell();
    } catch (std::exception& e){
        std::cerr << e.what();
        perror(" :");
    }
    return 0;
}
