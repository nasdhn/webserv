#include "../../includes/Response.hpp"
#include "Request.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <algorithm> 

Response::Response() 
    : _req(NULL), 
      _server(NULL), 
      _location(NULL), 
      _is_fd(false), 
      _status(200), 
      _fd_body(-1), 
      _cgi_pid(-1),
      _cgi_input_fd(-1),
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
      _cgi_input_fd(-1),
      _body("")
{
    _headers["Server"] = "Webserv/1.0";
    _headers["Connection"] = "keep-alive";
    _build();
}

Response::~Response()
{
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
        _cgi_input_fd = other._cgi_input_fd;
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

int Response::getCgiInputFD() const
{
    return _cgi_input_fd;
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

std::string Response::_getMimeType(std::string fullPath)
{
    std::string ext = _getExtension(fullPath);
    if (ext == ".html") return "text/html";
    if (ext == ".css") return "text/css";
    if (ext == ".js") return "application/javascript";
    if (ext == ".png") return "image/png";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".ico") return "image/x-icon";
    if (ext == ".txt") return "text/plain";
    return "text/plain";
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
        dup2(fd_out[1], STDERR_FILENO);
        close(fd_out[1]);

        close(fd_in[1]);
        dup2(fd_in[0], STDIN_FILENO);
        close(fd_in[0]);

        std::string ext = _getExtension(fullPath);
        std::string bin;
        char *arg[4];

        if (ext == ".py") {
            bin = "/usr/bin/python3";
            arg[0] = (char *)bin.c_str();
            arg[1] = (char *)"-u";
            arg[2] = (char *)fullPath.c_str();
            arg[3] = NULL;
        }
        else if (ext == ".cgi" || ext == ".php") 
        {
            bin = "/usr/bin/php-cgi";
            arg[0] = (char *)bin.c_str();
            arg[1] = (char *)fullPath.c_str();
            arg[2] = NULL;
            arg[3] = NULL;
        }
        else
            exit(1);

        std::vector<std::string> envs;
        envs.push_back("REQUEST_METHOD=" + _req->getMethod());
        envs.push_back("QUERY_STRING=" + _req->getQuery());
        envs.push_back("PATH_INFO=" + _req->getPath());
        envs.push_back("SCRIPT_FILENAME=" + fullPath);
        envs.push_back("REDIRECT_STATUS=200");
        
        if (!_req->getBody().empty()) {
             std::stringstream ss;
             ss << _req->getBody().size();
             envs.push_back("CONTENT_LENGTH=" + ss.str());
        }
        if (!_req->getHeader("Content-Type").empty())
            envs.push_back("CONTENT_TYPE=" + _req->getHeader("Content-Type"));

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
        if (_req->getMethod() == "POST" && !_req->getBody().empty()) 
        {
            _setNonBlocking(fd_in[1]);
            _cgi_input_fd = fd_in[1];
        }
        else
        {
            close(fd_in[1]);
            _cgi_input_fd = -1;
        }

        _setNonBlocking(fd_out[0]);
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

std::string readHtml(std::string path)
{
    std::ifstream file(path.c_str());
    if (file.is_open()) {
        std::stringstream buffer; buffer << file.rdbuf(); return buffer.str();
    }
    return "";
}

std::string Response::_generateAutoIndex(const std::string& fullPath, const std::string& uri)
{
    std::string html = "<html><body><h1>Index of " + uri + "</h1></body></html>";
    html += "<h1>Index of " + uri + "</h1><hr><ul>";
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(fullPath.c_str())) != NULL)
    {
        while((ent = readdir(dir)) != NULL)
        {
            std::string file_name = ent->d_name;
            if (file_name == ".")
            {
                continue;
            }
            std::string link = file_name;
            if (ent->d_type == DT_DIR)
                file_name += "/";
            std::string href = uri;
            if (href[href.size() - 1] != '/')
                href += "/";
            href += link;

            html += "<li><a href=\"" + link + "\">" + file_name + "</a></li>";
        }
        closedir(dir);
    } 
    else 
    {
        return "<html><body><h1>Error</h1><p>Could not open directory.</p></body></html>";
    }

    html += "</ul><hr></body></html>";
    return html;
}

void Response::_build()
{
    struct stat info;
    std::string fullPath;
    std::string root;
    if (_server && _req->getBody().size() > _server->getMaxSize())
    {
        setStatus(413);
        setBody(_getErrorPageContent(413));
        return;
    }
    if (_location)
    {
        int redirCode = _location->getRedir().code; 
        std::string redirUrl = _location->getRedir().link;
        if (redirCode != 0 && !redirUrl.empty())
        {
            setStatus(redirCode);
            setHeader("Location", redirUrl);
            setHeader("Connection", "keep-alive");
            setBody("<html><body><h1>Moved</h1><p>The document has moved <a href=\"" + redirUrl + "\">here</a>.</p></body></html>");
            return;
        }
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
    if (_location && !_location->getRoot().empty())
        root = _location->getRoot();
    else if (_server && !_server->getRoot().empty())
        root = _server->getRoot();
    else
        root = "./www";

    fullPath = root + _req->getPath();
    std::string ext = _getExtension(fullPath);
    if (_req->getMethod() == "DELETE")
    {
        return _ft_delete(fullPath);
    }
    if (_req->getMethod() == "POST" && ext != ".py" && ext != ".php" && ext != ".cgi")
    {
        std::ofstream file(fullPath.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

        if (file.is_open())
        {
            file.write(_req->getBody().c_str(), _req->getBody().size());
            if (file.bad() || file.fail()) 
            {
                file.close();
                setStatus(500);
                setBody(_getErrorPageContent(500));
                setHeader("Connection", "close");
                return;
            }
            file.close();
            setStatus(201);
            setBody("<html><body><h1>201 Created</h1><p>File created successfully.</p></body></html>");
            setHeader("Location", _req->getPath());
            setHeader("Connection", "keep-alive");
        }
        else
        {
            setStatus(403);
            setBody(_getErrorPageContent(403));
            setHeader("Connection", "keep-alive");
        }
        return;
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
            setBody(_generateAutoIndex(fullPath, _req->getPath()));
            return;
        }

        std::vector<std::string> indexes;
        if (_location && !_location->getIndex().empty())
            indexes = _location->getIndex();
        else if (_server && !_server->getIndex().empty())
            indexes = _server->getIndex();

        if (!indexes.empty())
        {
            for (size_t i = 0; i < indexes.size(); ++i)
            {
                std::string indexPath = fullPath;
                if (indexPath[indexPath.length() - 1] != '/')
                    indexPath += "/";
                indexPath += indexes[i];

                std::ifstream f(indexPath.c_str());
                if (f.good())
                {
                    setStatus(200);
                    setBody(readHtml(indexPath));
                    f.close();
                    return;
                }
                f.close();
            }
        }
        
        setStatus(403);
        setBody(_getErrorPageContent(403));
        return;
    }
    if (ext == ".py" || ext == ".php" || ext == ".cgi")
    {
        int fd_cgi = _execCGI(fullPath);
        
        if (fd_cgi == -1)
        {
            setStatus(500);
            setBody(_getErrorPageContent(500));
            return;
        }
        setStatus(200);
        setBodyfd(fd_cgi);
        setHeader("Connection", "close");
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
        _setNonBlocking(fd);

        setStatus(200);
        setBodyfd(fd);
        
        std::stringstream ss;
        ss << info.st_size;
        setHeader("Content-Length", ss.str());
        setHeader("Content-Type", _getMimeType(fullPath));
    }
    else
    {
        setStatus(405);
        setBody(_getErrorPageContent(405));
        setHeader("Connection", "close");
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