#include "../../includes/Response.hpp"
#include "Request.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include <unistd.h>
#include <fcntl.h>

Response::Response() 
    : _req(NULL), 
      _server(NULL), 
      _location(NULL), 
      _is_fd(false), 
      _status(200), 
      _fd_body(-1), 
      _cgi_pid(-1), 
      _body("")
{
    _headers["Server"] = "Webserv/1.0";
    _headers["Connection"] = "keep-alive";
}

Response::Response(Request& req, Server* server, Location* location) 
    : _req(&req),
      _server(server),
      _location(location),
      _is_fd(false), 
      _status(200), 
      _fd_body(-1), 
      _cgi_pid(-1), 
      _body("")
{
    _headers["Server"] = "Webserv/1.0";
    _headers["Connection"] = "keep-alive";
    _build();
}

Response::~Response()
{
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

void Response::_setNonBlocking(int fd)
{
    if (fd != -1)
        fcntl(fd, F_SETFL, O_NONBLOCK);
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

pid_t Response::get_pid() const
{
    return (_cgi_pid);
}

bool Response::is_fd() const
{
    return (_is_fd);
}

int Response::get_body_fd() const
{
    return _fd_body;
}

std::string Response::_getExtension(std::string path)
{
    size_t pos = path.find_last_of(".");
    if (pos == std::string::npos) 
        return "";
    return path.substr(pos);
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

    res << "\r\n";
    return res.str();
}

std::string Response::getHeaderString() const
{
    return get_header();
}

int Response::_execCGI(std::string fullPath)
{
    int fd_out[2];
    int fd_in[2];

    if (pipe(fd_out) == -1 || pipe(fd_in) == -1)
    {
        std::cerr << "Error: pipe" << std::endl;
        return (-1);
    }
    //non bloquant sur les fd
    _setNonBlocking(fd_out[0]);
    _setNonBlocking(fd_out[1]);
    _setNonBlocking(fd_in[0]);
    _setNonBlocking(fd_in[1]);

    setpid(fork());
    if (get_pid() < 0)
    {
        std::cerr << "Error : fork" << std::endl;
        return (-1);
    }

    if (get_pid() == 0)
    {
        close(fd_out[0]);
        dup2(fd_out[1], STDOUT_FILENO);
        close(fd_out[1]);

        close(fd_in[1]);
        dup2(fd_in[0], STDIN_FILENO);
        close(fd_in[0]);

        std::string ext = _getExtension(fullPath);
        std::string bin;
        
        if (ext == ".py")
            bin = "/usr/bin/python3";
        else if (ext == ".cgi" || ext == ".php")
            bin = "/usr/bin/php-cgi";
        else
            exit(1);

        char *arg[3];
        arg[0] = (char *)bin.c_str();
        arg[1] = (char *)fullPath.c_str();
        arg[2] = NULL;

        std::vector<std::string> envs;
        envs.push_back("REQUEST_METHOD=" + _req->getMethod());
        envs.push_back("QUERY_STRING=" + _req->getQuery());
        envs.push_back("PATH_INFO=" + _req->getPath());
        
        if (!_req->getBody().empty()) {
             std::stringstream ss;
             ss << _req->getBody().size();
             envs.push_back("CONTENT_LENGTH=" + ss.str());
        }

        char **envp = new char*[envs.size() + 1];
        for (size_t i = 0; i < envs.size(); i++) {
            envp[i] = (char*)envs[i].c_str();
        }
        envp[envs.size()] = NULL;

        execve(bin.c_str(), arg, envp);
        perror("execve FAILED");
        delete[] envp;
        exit(1);
    }
    else
    {
        close(fd_out[1]);
        close(fd_in[0]);

        if (_req->getMethod() == "POST" && !_req->getBody().empty()) {
            write(fd_in[1], _req->getBody().c_str(), _req->getBody().size());
        }
        close(fd_in[1]);

        return (fd_out[0]);
    }
}

void Response::_ft_delete(std::string fullPath)
{
    struct stat info;
    if (stat(fullPath.c_str(), &info) != 0)
    {
        setStatus(404);
        setBody(_getErrorPageContent(404));
        return;
    }
    if (unlink(fullPath.c_str()) == 0)
    {
        setStatus(204);
        setBody(""); 
        return;
    }
    
    setStatus(403);
    setBody(_getErrorPageContent(403));
    return;
}

std::string Response::_getErrorPageContent(int code)
{
    if (_server)
    {
        std::map<unsigned int, std::string> errorPages = _server->getErrorPage();
        if (errorPages.count(code) > 0)
        {
            std::string path = errorPages[code];
            std::ifstream file(path.c_str());
            if (file.is_open())
            {
                std::stringstream buffer;
                buffer << file.rdbuf();
                return buffer.str();
            }
        }
    }
    std::stringstream ss;
    ss << "<html><head><title>Error</title></head><body><center><h1>" << code << "</h1></center></body></html>";
    return ss.str();
}

void Response::_build()
{
    struct stat info;
    std::string fullPath;

    if (_server && _req->getBody().size() > _server->getMaxSize())
    {
        setStatus(413);
        setBody(_getErrorPageContent(413));
        return;
    }

    if (_location)
    {
        std::vector<std::string> methods = _location->getMethods();
        bool allowed = false;
        if (methods.empty()) allowed = true;
        else {
            for (size_t i = 0; i < methods.size(); i++) {
                if (methods[i] == _req->getMethod()) {
                    allowed = true;
                    break;
                }
            }
        }
        if (!allowed) {
            setStatus(405);
            setBody(_getErrorPageContent(405));
            return ;
        }
    }

    if (_location)
        fullPath = _location->getRoot() + _req->getPath();
    else if (_server)
        fullPath = _server->getRoot() + _req->getPath();
    else
        fullPath = "./www" + _req->getPath();

    if (_req->getMethod() == "DELETE")
    {
        return _ft_delete(fullPath);
    }

    if (stat(fullPath.c_str(), &info) != 0)
    {
        setStatus(404);
        setBody(_getErrorPageContent(404));
        return;
    }

    if (S_ISDIR(info.st_mode))
    {
        if (_location && _location->getAutoIndex() == 1)
        {
             setStatus(200);
             setBody("<h1>Index of " + _req->getPath() + "</h1>");
             return;
        }
        setStatus(403);
        setBody(_getErrorPageContent(403));
        return;
    }

    std::string ext = _getExtension(fullPath);
    if (ext == ".py" || ext == ".php" || ext == ".cgi")
    {
        int content = _execCGI(fullPath);
        
        if (content == -1)
        {
            setStatus(500);
            setBody(_getErrorPageContent(500));
            return;
        }
        setStatus(200);
        setBodyfd(content);
        return;
    }

    if (_req->getMethod() == "GET")
    {
        int fd = open(fullPath.c_str(), O_RDONLY);
        if (fd == -1)
        {
            setStatus(403);
            setBody(_getErrorPageContent(403));
            return;
        }
        //non bloquant sur le fichier
        _setNonBlocking(fd);

        setStatus(200);
        setBodyfd(fd);
        
        std::stringstream ss;
        ss << info.st_size;
        setHeader("Content-Length", ss.str());
    }
    else
    {
        setStatus(405);
        setBody(_getErrorPageContent(405));
    }
    return;
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