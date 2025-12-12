#include "Server.hpp"

Server::Server()
{

}

Server::Server(const Server& other)
{
	(void)other;
}
Server& Server::operator=(const Server& other)
{	
	(void)other;
	return (*this);
}

Server::~Server()
{

}

void Server::setupServ()
{
	int socketServer = socket(AF_INET, SOCK_STREAM, 0);
	if (socketServer == -1)
		throw std::runtime_error("Error : Socket's initialisation failed !");

	int opt = 1;
	if (setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		throw std::runtime_error("Error : Set Socket's initialisation failed !");

	struct sockaddr_in address = {};
	address.sin_family = AF_INET;
	address.sin_port = htons(8080); // a completer avec parsing
	address.sin_addr.s_addr = INADDR_ANY;
	// address.sin_zero[(sizeof(address))];

	int flag = fcntl(socketServer, F_GETFL, 0);
	if (flag == -1)
	{	
		close(socketServer);
		throw std::runtime_error("Error : Can't get socket flags !");
	}

	if (fcntl(socketServer, F_SETFL, flag | O_NONBLOCK) == -1)
	{	
		close(socketServer);
		throw std::runtime_error("Error : Set non blocking's initialisation failed !");
	}

	if (bind(socketServer, (struct sockaddr*)&address, sizeof(address)) == -1)
	{	
		close(socketServer);
		throw std::runtime_error("Error : Bind failed !");
	}
	std::cout << "Bind" << std::endl;


	if (listen(socketServer, 10) == -1)
	{	
		close(socketServer);
		throw std::runtime_error("Error : Listen failed !");
	}
	std::cout << "Mode ecoute" << std::endl;

	socklen_t sizeaddr = sizeof(address);

	// integration de poll
	struct pollfd sfd;
	sfd.fd = socketServer;
	sfd.events = POLLIN;
	sfd.revents = 0;
	_fd.push_back(sfd);


	while (1)
	{
		// integration de poll
		if (poll(&_fd[0], _fd.size(), -1) == -1)
			throw std::runtime_error("Error : Poll's initialisation failed !");

		for (unsigned int i = 0; i < _fd.size(); i++)
		{
			if (_fd[i].revents & POLLIN)
			{
				if (_fd[i].fd == socketServer)
				{
					int clientSocket = accept(socketServer, (struct sockaddr*)&address, &sizeaddr); // a changer pour mettre le nom dune struct client avec info parsing
					if (clientSocket == -1)
						throw std::runtime_error("Error : Client Socket's initialisation failed !");

					int flag = fcntl(clientSocket, F_GETFL, 0);
					if (flag == -1)
					{	
						close(clientSocket);
						throw std::runtime_error("Error : Can't get socket flags !");
					}

					if (fcntl(clientSocket, F_SETFL, flag | O_NONBLOCK) == -1)
					{	
						close(clientSocket);
						throw std::runtime_error("Error : Set non blocking's initialisation failed !");
					}

					struct pollfd clientFD;
					clientFD.fd = clientSocket;
					clientFD.events = POLLIN;
					clientFD.revents = 0;
					_fd.push_back(clientFD);

					// DEBUG
					std::cout << "Nouveau client connecté sur le FD " << clientSocket << std::endl;
				}
				else
				{
					// temporaire pour tester
					char buffer[1024] = {0};
					int ret = recv(_fd[i].fd, buffer, sizeof(buffer) - 1, 0);  // utiliser une map pour mettre le buff pas fini avec l identifiant jusqu'a tout recevoir
					if (ret <= 0)
					{
						std::cout << "Déconnexion.." << std::endl;
						close(_fd[i].fd);
						_fd.erase(_fd.begin() + i);
						i--;
					}
					if (ret > 0)
					{
						buffer[ret] = '\0';
						std::cout << "Message recu : " << buffer << std::endl;
					}

					std::cout << "client parle.." << std::endl;
				}
			}
		}

	}



	close(socketServer);
}
