#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <map>
#include <iostream>
#include <sstream>
#include "Request.hpp"
#include "WebServ.hpp"
#include "Location.hpp"
#include "Response.hpp"

class WebServ;
class Server;

class Client {

    public :
        Client(int fd, WebServ* webServ);
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

        void setFileFD(int n);
        void closeFile();
        bool& getRoutingDone();
        void reset();

        Request& getRequest();
        int& getFileFD();





    private :
        int _id;
        Response _response;
        std::string _responseStr; // contientdra header + body
        int _fileFD;
        bool _readyToSend;
        unsigned long _byteSend;
        time_t _lastTime;
        bool _routingDone;

        Request _request;
        Server* _server;
        const Location* _location;
        WebServ* _webServ;
};


#endif