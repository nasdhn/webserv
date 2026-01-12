#include "../../includes/Response.hpp"
#include <unistd.h>

Response::Response() : _status(200), _body("") {
    _headers["Server"] = "Webserv/1.0";
}

Response::~Response() {
    if (_is_fd && _fd_body != -1) {
        close(_fd_body);
    }
    if (_cgi_pid == -1)
    {
        int status;
        waitpid(_cgi_pid, &status, 0);
    }
}

Response::Response(const Response &other) {
    *this = other;
}

Response &Response::operator=(const Response &other) {
    if (this != &other) {
        _status = other._status;
        _body = other._body;
        _headers = other._headers;
    }
    return *this;
}

void Response::setStatus(int code) {
    _status = code;
}

void Response::setHeader(const std::string& key, const std::string& value) {
    _headers[key] = value;
}

void Response::setBody(const std::string& body) {
    _is_fd = false;
    _body = body;
    std::stringstream ss;
    ss << body.length();
    setHeader("Content-Length", ss.str());
}

void Response::setBodyfd(int fd)
{
    _is_fd = true;
    _fd_body = fd;
}

std::string Response::_getStatusMessage(int code) const {
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 500: return "Internal Server Error";
        default: return "Unknown Status";
    }
}

std::string Response::get_header() const 
{
    std::stringstream res;
    res << "HTTP/1.1 " << _status << " " << _getStatusMessage(_status) << "\r\n";
    std::map<std::string, std::string>::const_iterator it;
    for (it = _headers.begin(); it != _headers.end(); ++it) {
        res << it->first << ": " << it->second << "\r\n";
    }
    res << "\r\n";
    return res.str();
}

void Response::setpid(pid_t pid)
{
    _cgi_pid = pid;
}

bool Response::is_fd()
{
    return (_is_fd);
}

int Response::get_body_fd()
{
    return _fd_body;
}

std::string Response::get_body_string()
{
    return _body;
}