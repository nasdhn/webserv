#include "Serveur.hpp"

int main()
{
	int socketServer = socket(AF_INET, SOCK_STREAM, 0);

	int opt = 1;
	setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_zero[(sizeof(address))];

	bind(socketServer, (struct sockaddr*)&address, sizeof(address));
	std::cout << "Bind" << std::endl;


	listen(socketServer, 10);
	std::cout << "Mode ecoute" << std::endl;

	socklen_t sizeaddr = sizeof(address);

	int client = accept(socketServer, (struct sockaddr*)&address, &sizeaddr); // a changer pour mettre le nom dune struct client

	std::cout << "Connecte" << std::endl;

	close(client);
	close(socketServer);

	return 0;

}


