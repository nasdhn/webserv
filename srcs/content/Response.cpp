#include "../../includes/Response.hpp"
#include <unistd.h>
#include <fcntl.h>

Response::Response() : _is_fd(false), _status(200), _fd_body(-1), _cgi_pid(-1), _body("") 
{
    _headers["Server"] = "Webserv/1.0";
    _headers["Connection"] = "keep-alive";
}

Response::~Response()
{
    if (_is_fd && _fd_body != -1) {
        close(_fd_body);
    }
    if (_cgi_pid != -1) {
        int status;
        int result = waitpid(_cgi_pid, &status, WNOHANG);
        if (result == 0) {
            kill(_cgi_pid, SIGKILL);
            waitpid(_cgi_pid, &status, 0);
        }
    }
}

Response::Response(const Response &other)
{
    *this = other;
}

Response &Response::operator=(const Response &other)
{
    if (this != &other) {
        _is_fd = other._is_fd;
        _status = other._status;
        _fd_body = other._fd_body;
        _cgi_pid = other._cgi_pid;
        _body = other._body;
        _headers = other._headers;
    }
    return *this;
}

void Response::setStatus(int code)
{
    _status = code;
}

void Response::setHeader(const std::string& key, const std::string& value)
{
    _headers[key] = value;
}

void Response::setBody(const std::string& body)
{
    _is_fd = false;
    _body = body;
    _fd_body = -1;
    
    std::stringstream ss;
    ss << body.length();
    setHeader("Content-Length", ss.str());
}

void Response::setBodyfd(int fd)
{
    _is_fd = true;
    _fd_body = fd;
}

void Response::setpid(pid_t pid)
{
    _cgi_pid = pid;
}

bool Response::is_fd() const
{
    return (_is_fd);
}

int Response::get_body_fd() const
{
    return _fd_body;
}

std::string Response::get_body_string() const
{
    return _body;
}

int Response::getStatus() const
{
    return _status;
}

std::string Response::get_header() const 
{
    std::stringstream res;
    res << "HTTP/1.1 " << _status << " " << _getStatusMessage(_status) << "\r\n";
    
    std::map<std::string, std::string>::const_iterator it;
    for (it = _headers.begin(); it != _headers.end(); ++it) {
        res << it->first << ": " << it->second << "\r\n";
    }
    return res.str();
}

std::string Response::getHeaderString() const
{
    return get_header();
}

std::string Response::_getStatusMessage(int code) const
{
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 413: return "Payload Too Large";
        case 500: return "Internal Server Error";
        case 502: return "Bad Gateway";
        default: return "Unknown Status";
    }
}