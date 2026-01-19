#include <string>
#include <unistd.h>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../../includes/Response.hpp"

#include <fstream>
#include <sstream>

std::string getErrorPageContent(int code, FakeRequest req) {
    if (req.error_pages.count(code) > 0) {
        std::string path = req.error_pages[code];
        std::ifstream file(path.c_str());
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        }
    }
    std::stringstream ss;
    ss << "<html><head><title>Error</title></head>";
    ss << "<body><center><h1>" << code << "</h1></center>";
    
    if (code == 404) ss << "<center>Not Found</center>";
    else if (code == 403) ss << "<center>Forbidden</center>";
    else if (code == 405) ss << "<center>Method Not Allowed</center>";
    else if (code == 500) ss << "<center>Internal Server Error</center>";
    
    ss << "</body></html>";
    return ss.str();
}

std::string getExtention(std::string path)
{
	std::string ext;
	int empl = path.find_last_of(".");
	if (empl == std::string::npos) 
        return "";
	ext = path.substr(empl);
	return ext;
}

int execCGI(FakeRequest req, pid_t &pid)
{
	int fd_out[2];
	int fd_in[2];
	if (pipe(fd_out) == -1 || pipe(fd_in))
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
		//process enfant
		close(fd_out[0]);
		dup2(fd_out[1], STDOUT_FILENO);
		close(fd_out[1]);
		close (fd_in[1]);
		dup2(fd_out[0], STDOUT_FILENO);
		close(fd_out[1]);
		std::string ext = getExtention(req.path);
		char *arg[3];
		std::string bin;
		if (ext == ".py")
		{
			bin = "/usr/bin/python3";
		}
		else if (ext == ".cgi" || ext == ".php")
		{
			bin = "/usr/bin/php-cgi";
		}
		else
		{
			exit (0);
		}
		arg[0] = (char *)bin.c_str();
		arg[1] = (char *)req.path.c_str();
		arg[2] = NULL;
		std::vector<std::string> envs;
    	envs.push_back("REQUEST_METHOD=" + req.method);
    	envs.push_back("QUERY_STRING=" + req.query);
    	envs.push_back("PATH_INFO=" + req.path);
		char **envp = new char*[envs.size() + 1];
    	for (size_t i = 0; i < envs.size(); i++) {
        	envp[i] = (char*)envs[i].c_str();
    	}
    	envp[envs.size()] = NULL;
		execve(bin.c_str(), arg, envp);
		perror("execve FAILED");
		delete[] envp;
		exit (1);
	}
	else
	{
		//process parent
		close(fd_out[1]);
		close(fd_in[0]);
		if (req.method == "POST" && !req.body.empty()) {
            write(fd_in[1], req.body.c_str(), req.body.size());
        }
		close(fd_in[1]);
		return (fd_out[0]);
	}
}

Response ft_delete(Response res, FakeRequest req)
{
	struct stat info;
	if (stat(req.path.c_str(), &info) != 0)
	{
		res.setStatus(404);
		res.setBody("<h1>404 Not Found</h1>");
		return res;
	}
	if (unlink(req.path.c_str()) == 0)
	{
		res.setStatus(204);
		res.setBody("<h1>204 No Content</h1>");
		return res;
	}
	return (res.setStatus(403), res);
}

Response BuildResponse(FakeRequest req)
{
	Response res;
	std::string body;
	struct stat info;
	if (req.method == "DELETE")
	{
		return ft_delete(res, req);
	}
	if (stat(req.path.c_str(), &info) != 0)
	{
		res.setStatus(404);
		res.setBody("<h1>404 Not Found</h1>");
		return res;
	}
	if (S_ISDIR(info.st_mode))
	{
		res.setStatus(200);
		res.setBody("<h1>It is a folder</h1>");
		return res;
	}
	std::string ext = getExtention(req.path);
	int content;
	if (ext == ".py" || ext == ".php" || ext == ".cgi")
	{
		pid_t pid;
		content = execCGI(req, pid);
		if (content == -1)
		{
			res.setStatus(500);
			return res;
		}
		res.setStatus(200);
		res.setBodyfd(content);
		res.setpid(pid);
		return res;
	}
	if (req.method == "GET")
	{
		int fd = open(req.path.c_str(), O_RDONLY);
		if (fd == -1)
		{
			res.setStatus(403);
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
	}
	return res;
}