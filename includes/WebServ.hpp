#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sys/poll.h>
#include <vector>
#include <map>
#include <fcntl.h>
#include "Client.hpp"
#include <memory>
#include <algorithm>
#include <csignal>
#include <cerrno>
#include "Server.hpp"

class Server;

struct SocketInfo {
    std::string ip;
    int port;
};

class WebServ {

	public :
		WebServ(std::vector<Server> &serv);
		WebServ(const WebServ& other);
		WebServ& operator=(const WebServ& other);
		~WebServ();
		std::vector<struct pollfd> getFD();

		void servInit(std::string ip, int port);
		void setupServ();
		bool sendResponse(Client *client, struct pollfd &pfd);
		void checkTimeOut();
		bool isServerSocket(int fd);
		void cleanAll();

		void printLog(const std::string &msg, const std::string &color);
		std::string intToStr(int n);


	private :
		std::vector<struct pollfd> _fd;
		std::map<int, Client*> _clients;
		std::vector<int> _serverSockets;
		bool _readyToSend;
};

void signalHandler(int sig);


#endif