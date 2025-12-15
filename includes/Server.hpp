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

class Server {

	public :
		Server();
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();
		std::vector<struct pollfd> getFD();

		int servInit();
		void setupServ();

	private :
		std::vector<struct pollfd> _fd;
		std::map<int, Client*> _clients;
};


#endif