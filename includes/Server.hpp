#ifndef SERVEUR_HPP
#define SERVEUR_HPP

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

class Server {

	public :
		Server();
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();
		std::vector<struct pollfd> getFD();

		void servInit(int port);
		void setupServ();
		void sendResponse(Client *client, struct pollfd &pfd);
		void checkTimeOut();
		bool isServerSocket(int fd);
		void cleanAll();


	private :
		std::vector<struct pollfd> _fd;
		std::map<int, Client*> _clients;
		std::vector<int> _serverSockets;
		bool _readyToSend;

		// provisoir le temps d'avoir le parsing
		std::vector<int> _port;
};

void signalHandler(int sig);


#endif