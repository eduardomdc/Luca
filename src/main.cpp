#include <iostream>
#include "client.hpp"

#define PORT 8000

int main(){
    try{
        Client client = Client(PORT);
        client.send_msg("Hello");
    } catch (std::exception& e){
        std::cerr << e.what() <<std::endl;
    }
    return 0;
}
