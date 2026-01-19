#include <string>
#include <unistd.h>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "../../includes/Response.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "Location.hpp"

std::string getExtention(std::string path)
{
    size_t pos = path.find_last_of(".");
    if (pos == std::string::npos) 
        return "";
    return path.substr(pos);
}

int execCGI(Request req, std::string fullPath, pid_t &pid)
{
    int fd_out[2];
    int fd_in[2];

    if (pipe(fd_out) == -1 || pipe(fd_in) == -1)
    {
        std::cerr << "Error: pipe" << std::endl;
        return (-1);
    }

    pid = fork();
    if (pid < 0)
    {
        std::cerr << "Error : fork" << std::endl;
        return (-1);
    }

    if (pid == 0)
    {
        close(fd_out[0]);
        dup2(fd_out[1], STDOUT_FILENO);
        close(fd_out[1]);

        close(fd_in[1]);
        dup2(fd_in[0], STDIN_FILENO);
        close(fd_in[0]);

        std::string ext = getExtention(fullPath);
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
        envs.push_back("REQUEST_METHOD=" + req.getMethod());
        envs.push_back("QUERY_STRING=" + req.getQuery());
        envs.push_back("PATH_INFO=" + req.getPath());
        
        if (!req.getBody().empty()) {
             std::stringstream ss;
             ss << req.getBody().size();
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

        if (req.getMethod() == "POST" && !req.getBody().empty()) {
            write(fd_in[1], req.getBody().c_str(), req.getBody().size());
        }
        close(fd_in[1]);

        return (fd_out[0]);
    }
}

Response ft_delete(Response res, std::string fullPath, Server *server)
{
    struct stat info;
    if (stat(fullPath.c_str(), &info) != 0)
    {
        res.setStatus(404);
        res.setBody(getErrorPageContent(404, server));
        return res;
    }
    if (unlink(fullPath.c_str()) == 0)
    {
        res.setStatus(204);
        res.setBody(""); 
        return res;
    }
    
    res.setStatus(403);
    res.setBody(getErrorPageContent(403, server));
    return res;
}

std::string getErrorPageContent(int code, Server *server)
{
    if (server)
    {
        std::map<unsigned int, std::string> errorPages = server->getErrorPage();
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

Response BuildResponse(Request req, Server *server,Location *location)
{
    Response res;
    struct stat info;
    std::string fullPath;

    if (server && req.getBody().size() > server->getMaxSize())
    {
        res.setStatus(413);
        res.setBody(getErrorPageContent(413, server));
        return res;
    }

    if (location)
    {
        std::vector<std::string> methods = location->getMethods();
        bool allowed = false;
        if (methods.empty()) allowed = true;
        else {
            for (size_t i = 0; i < methods.size(); i++) {
                if (methods[i] == req.getMethod()) {
                    allowed = true;
                    break;
                }
            }
        }
        if (!allowed) {
            res.setStatus(405);
            res.setBody(getErrorPageContent(405, server));
            return res;
        }
    }

    if (location)
        fullPath = location->getRoot() + req.getPath();
    else if (server)
        fullPath = server->getRoot() + req.getPath();
    else
        fullPath = "./www" + req.getPath();

    if (req.getMethod() == "DELETE")
    {
        return ft_delete(res, fullPath, server);
    }

    if (stat(fullPath.c_str(), &info) != 0)
    {
        res.setStatus(404);
        res.setBody(getErrorPageContent(404, server));
        return res;
    }

    if (S_ISDIR(info.st_mode))
    {
        if (location && location->getAutoIndex())
        {
             res.setStatus(200);
             res.setBody("<h1>Index of " + req.getPath() + "</h1>");
             return res;
        }
        res.setStatus(403);
        res.setBody(getErrorPageContent(403, server));
        return res;
    }

    std::string ext = getExtention(fullPath);
    if (ext == ".py" || ext == ".php" || ext == ".cgi")
    {
        pid_t pid;
        int content = execCGI(req, fullPath, pid);
        
        if (content == -1)
        {
            res.setStatus(500);
            res.setBody(getErrorPageContent(500, server));
            return res;
        }
        res.setStatus(200);
        res.setBodyfd(content);
        res.setpid(pid);
        return res;
    }

    if (req.getMethod() == "GET")
    {
        int fd = open(fullPath.c_str(), O_RDONLY);
        if (fd == -1)
        {
            res.setStatus(403);
            res.setBody(getErrorPageContent(403, server));
            return res;
        }
        res.setStatus(200);
        res.setBodyfd(fd);
        
        std::stringstream ss;
        ss << info.st_size;
        res.setHeader("Content-Length", ss.str());
    }
    else
    {
        res.setStatus(405);
        res.setBody(getErrorPageContent(405, server));
    }
    return res;
}
