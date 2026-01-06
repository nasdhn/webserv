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

std::string execCGI(std::string path)
{
	int fd[2];
	if (pipe(fd) == -1)
	{
		std::cerr << "Error: pipe" << std::endl;
	}
	pid_t pid = fork();
	if (pid < 0)
	{
		std::cerr << "Error : fork" << std::endl;
		return (NULL);
	}
	if (pid == 0)
	{
		//process enfant
		close(fd[0]);
		
	}
	else
	{
		//process parent
		close(fd[1]);
	}
}

std::string getExtention(std::string path)
{
	std::string ext;
	int empl = path.find_last_of(".");
	ext = path.substr(empl);
	return ext;
}

Response BuildResponse(std::string path)
{
	Response res;
	std::string body;
	struct stat info;
	if (stat(path.c_str(), &info) != '\0')
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
	std::string ext = getExtention(path);
	std::string content;
	if (ext == ".py" || ext == ".php" || ext == ".cgi")
	{
		content = execCGI(path);
		res.setStatus(200);
		res.setBody(content);
		return res;
	}
	int fd = open(path.c_str(), O_RDONLY);
	if (fd == -1)
	{
		res.setStatus(403);
		return res;
	}
	int bytesread;
	char buffer[4096];
	while(bytesread = read(fd, buffer, sizeof(buffer)))
		content.append(buffer, bytesread);
	close(fd);
	res.setStatus(200);
	res.setBody(content);
	return res;
}