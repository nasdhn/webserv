#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <map>
#include <iostream>
#include <sstream>
#include "Request.hpp"

class Client {

    public :
        Client(int fd);
        Client(const Client& other);
        Client& operator=(const Client &other);
        ~Client();
    
        int getID() const;
        std::string& getResponse();
        bool& getReadyToSend();
        unsigned long& getByteSend();
        time_t& getLastTime();
        void setLastTime(time_t time);
        void processRequest(const char* buffer, int size);


        Request& getRequest();





    private :
        int _id;
        std::string _response;
        bool _readyToSend;
        unsigned long _byteSend;
        time_t _lastTime;

        Request _request;
};


#endif