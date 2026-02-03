#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h> 

class Request;
class Server;
class Location;

class Response {

    public:
        Response();
        Response(Request& req, Server* server, Location* location);
        Response(const Response &other);
        Response &operator=(const Response &other);
        ~Response();

        std::string get_header() const;
        std::string get_body_string() const;
        int         get_body_fd() const;
        bool        is_fd() const;
        int         getStatus() const;
        pid_t       get_pid() const;
        int         getCgiInputFD() const;

        void        setStatus(int code);
        void        setBody(const std::string& body);
        void        setHeader(const std::string& key, const std::string& value);
        void        setBodyfd(int fd);
        void        setpid(pid_t pid);
        std::string getHeaderString() const;
        std::string _getErrorPageContent(int code);

    private:
        Request* _req;
        Server* _server;
        Location* _location;

        bool        _is_fd;
        int         _status;
        int         _fd_body;
        pid_t       _cgi_pid;
        int         _cgi_input_fd;

        std::string _body;
        std::map<std::string, std::string> _headers;

        void        _build();
        void        _ft_delete(std::string fullPath);
        int         _execCGI(std::string fullPath);
        std::string _getExtension(std::string path);
        std::string _getMimeType(std::string fullPath);
        std::string _generateAutoIndex(const std::string& fullPath, const std::string& uri);
        std::string _getStatusMessage(int code) const;
        void        _setNonBlocking(int fd);
};

#endif