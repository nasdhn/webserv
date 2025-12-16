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
    
        int getID() const;
        std::string& getRequest();
        std::string& getHeader();

    private :
        int _id;
        std::string _request;
        std::string _header;
     
};


#endif