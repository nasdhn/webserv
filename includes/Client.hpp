#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <map>

class Client {

    public :
        Client();
        Client(const Client& other);
        Client& operator=(const Client &other);
        ~Client();

    private :
        std::map<int, std::string> _buff_fd;

};


#endif