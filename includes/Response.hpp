#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <signal.h>
#include <sys/wait.h>

class Response {
    public:
        Response();
        ~Response();
        Response(const Response &other);
        Response &operator=(const Response &other);
        void setStatus(int code);
        void setHeader(const std::string& key, const std::string& value);
        void setBody(const std::string& body);
        void setBodyfd(int fd);
        bool is_fd();
        void setpid(pid_t pid);
        std::string get_header() const;
        int get_body_fd();
        std::string get_body_string();
        

    private:
        bool _is_fd;
        int _status;
        int _fd_body;
        pid_t _cgi_pid;
        std::string _body;
        std::map<std::string, std::string> _headers;
        std::string _getStatusMessage(int code) const;
};

#endif