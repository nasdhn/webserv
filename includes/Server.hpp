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
#include <fcntl.h>

class Server {

	public :
		Server();
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();



		void setupServ();
		void setupClient();

	private :
		std::vector<struct pollfd> _fd;
};


#endif