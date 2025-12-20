#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <map>
#include <iostream>
#include <sstream>

class Client {

    public :
        Client(int fd);
        Client(const Client& other);
        Client& operator=(const Client &other);
        ~Client();
    
        int getID() const;
        std::string& getRequest();
        std::string& getHeader();
        std::string& getResponse();
        bool& getReadyToSend();
        unsigned long& getByteSend();
        time_t& getLastTime();
        void setLastTime(time_t time);

        // fonction provisoire a remplacer ou a suppr par celle de class Request
        int& getContentSizeInt();
        std::string& getContentSizeString();




    private :
        int _id;
        std::string _request;
        std::string _header;
        std::string _response;
        bool _readyToSend;
        unsigned long _byteSend;;
        time_t _lastTime;

        // variable provisoire a remplacer par celle de class Request
        std::string _contentLenght_str;
        int _contentSize_int;
     
};


#endif