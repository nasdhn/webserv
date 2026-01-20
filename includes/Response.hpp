#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

class Request;
class Server;
class Location;

class Response {
    public:
        Response();
        Response(Request& req, Server* server, Location* location);
        ~Response();
        Response(const Response &other);
        Response &operator=(const Response &other);

        void setStatus(int code);
        void setHeader(const std::string& key, const std::string& value);
        void setBody(const std::string& body);
        void setBodyfd(int fd);
        void setpid(pid_t pid);

        bool is_fd() const;
        int get_body_fd() const;
        pid_t get_pid() const;
        std::string get_body_string() const;
        std::string get_header() const;
        std::string getHeaderString() const;
        int getStatus() const;

    private:
        Request* _req;
        Server* _server;
        Location* _location;
        bool _is_fd;
        int _status;
        int _fd_body;
        pid_t _cgi_pid;
        std::string _body;
        std::map<std::string, std::string> _headers;

        void        _build();
        int         _execCGI(std::string fullPath);
        void        _ft_delete(std::string fullPath);

        std::string _getStatusMessage(int code) const;
        std::string _getErrorPageContent(int code);
        std::string _getExtension(std::string path);
        void        _setNonBlocking(int fd);
};

#endif