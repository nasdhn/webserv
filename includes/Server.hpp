#ifndef SERVEUR_HPP
#define SERVEUR_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

class Server {

	public :
		Server();
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();



		void setupServ();

	private :
};


#endif