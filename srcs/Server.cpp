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
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = INADDR_ANY;
	// address.sin_zero[(sizeof(address))];

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

	while (1)
	{
		int clientSocket = accept(socketServer, (struct sockaddr*)&address, &sizeaddr); // a changer pour mettre le nom dune struct client
		if (clientSocket == -1)
			throw std::runtime_error("Error : Client Socket's initialisation failed !");
		std::cout << "Connecte" << std::endl;

		close(clientSocket);
	}



	close(socketServer);
}

void Server::setupClient()
{

}