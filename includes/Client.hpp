#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <map>
#include <iostream>

class Client {

    public :
        Client(int fd);
        Client(const Client& other);
        Client& operator=(const Client &other);
        ~Client();
    
        int _id;
        std::string _request;

    private :



};


#endif