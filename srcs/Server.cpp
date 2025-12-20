#include "Server.hpp"

Server::Server()
{
}

Server::Server(const Server &other)
{
	_fd = other._fd;
	_clients = other._clients;
}
Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		_fd = other._fd;
		_clients = other._clients;
		_readyToSend = other._readyToSend;
	}
	return (*this);
}

Server::~Server()
{
}

std::vector<struct pollfd> Server::getFD()
{
	return _fd;
}

// a implementer apres pour split le code

void Server::sendResponse(Client *client, struct pollfd &pfd)
{
	std::string &msg = client->getResponse();
	unsigned long total_size = client->getResponse().size();
	unsigned long sent = client->getByteSend();

	int ret = send(pfd.fd, msg.c_str() + sent, total_size - sent, 0);

	if (ret > 0)
	{
		client->getByteSend() += ret;
		std::cout << ret << " octets envoyés..." << std::endl;

		if (client->getByteSend() >= total_size)
		{
			std::cout << "Envoi terminé pour le client " << pfd.fd << std::endl;
			client->getReadyToSend() = false;
			client->getByteSend() = 0;
			client->getRequest().clear();
			client->getResponse().clear();

			pfd.events = POLLIN;
		}
	}
	if (ret == -1)
	{
		std::cout << "Envoi echoué.. a retnter" << std::endl;
	}
}

int Server::servInit()
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
	if (bind(socketServer, (struct sockaddr *)&address, sizeof(address)) == -1)
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

	// integration de poll
	struct pollfd sfd;
	sfd.fd = socketServer;
	sfd.events = POLLIN;
	sfd.revents = 0;
	_fd.push_back(sfd);

	return (socketServer);
}

void Server::setupServ()
{
	int socketServer = servInit();

	struct sockaddr_in clientAddr = {};
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(8080); // a completer avec parsing
	clientAddr.sin_addr.s_addr = INADDR_ANY;
	// address.sin_zero[(sizeof(address))];
	socklen_t sizeaddr = sizeof(clientAddr);

	while (1)
	{
		// integration de poll
		if (poll(&_fd[0], _fd.size(), -1) == -1)
			throw std::runtime_error("Error : Poll's initialisation failed !");

		for (unsigned int i = 0; i < _fd.size(); i++)
		{
			if (_fd[i].revents & POLLIN)
			{
				// a chaque client on passe la dedans
				if (_fd[i].fd == socketServer)
				{
					int clientSocket = accept(socketServer, (struct sockaddr *)&clientAddr, &sizeaddr); // a changer pour mettre le nom dune struct client avec info parsing
					if (clientSocket == -1)
					{
						std::cerr << "Error : Client's connexion failed.." << std::endl;
						continue;
					}
					int flag = fcntl(clientSocket, F_GETFL, 0);
					if (flag == -1)
					{
						close(clientSocket);
						throw std::runtime_error("Error : Can't get socket flags !");
					}
					if (fcntl(clientSocket, F_SETFL, flag | O_NONBLOCK) == -1)
					{
						close(clientSocket);
						std::cerr << "Error fcntl : Client closed.." << std::endl;
						continue;
					}

					Client *newClient = new Client(clientSocket);
					_clients[clientSocket] = newClient;

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
					Client *client = _clients[_fd[i].fd];
					char buffer[1024] = {0};
					int ret = recv(_fd[i].fd, buffer, sizeof(buffer) - 1, 0);
					if (ret <= 0)
					{
						// si -1 check errno seulement ici si buffer vide pas grave si pas errno on coupe tout
						// si 0 on close
						std::cout << "Déconnexion.." << std::endl;
						close(_fd[i].fd);
						_clients.erase(_fd[i].fd);
						delete client;
						_fd.erase(_fd.begin() + i);
						i--;
					}
					if (ret > 0)
					{
						buffer[ret] = '\0';
						client->getRequest().append(buffer, ret);

						// DEBUG
						// std::cout << "Message recu : " << buffer << std::endl;
						// std::cout << "Message de id : " << client->getID() << " dans _client : " << client->getRequest() << std::endl;
						// std::cout << "Octet recu : " << ret << " | Total octet : " << client->getRequest().size() << std::endl;
						// DEBUG

						unsigned long pos = client->getRequest().find("\r\n\r\n");
						if (pos != std::string::npos)
						{
							client->getHeader() = client->getRequest().substr(0, pos + 4);
							client->getRequest().erase(0, pos + 4);

							// Faire une class request pour les request
							// code provisoire et variable a modifier une fois que la class sera faite
							// content_lenght et request
							std::string CL = "Content-Length:";
							unsigned long CL_pos = client->getHeader().find(CL);
							if (CL_pos != std::string::npos)
							{
								unsigned long endLine = client->getHeader().find('\n', CL_pos);
								client->getContentSizeString() = client->getHeader().substr(CL_pos + 16, endLine - (CL_pos + 16));

								std::istringstream iss(client->getContentSizeString());
								std::cout << "Content size string : " << client->getContentSizeString() << std::endl;
								iss >> client->getContentSizeInt();
								std::cout << "Le content lenght existe et la taille est : " << client->getContentSizeInt() << std::endl;

								// avec content lenght check le buffer si il a tout lu ou si il en reste c est ce qui va etre le body
								if ((size_t)client->getRequest().size() < (size_t)client->getContentSizeInt())
								{
									std::cout << "Body pas encore tout lu" << std::endl;
									continue;
								}
								else
								{
									client->getReadyToSend() = true;
									std::cout << "body entierement lu" << std::endl;
								}
							}
							else
							{
								// pas de content lenght donc direct passer a la reponse
								client->getReadyToSend() = true;
								std::cout << "pas de content leghnt" << std::endl;
							}
							// jusqu'ici

							if (client->getReadyToSend() == true)
								_fd[i].events = POLLOUT | POLLIN;

							// DEBUG
							std::cout << "Header : " << client->getHeader() << std::endl;
							std::cout << "Request apres le transfert : " << client->getRequest() << std::endl;

							std::cout << "Requete complete" << std::endl;
							// DEBUG
						}
						else
							continue;
					}

					// DEBUG
					std::cout << "client parle.." << std::endl;
					std::cout << _fd.size() << " FD dans la liste" << std::endl;
				}
			}
			if (_fd[i].revents & POLLOUT)
			{
				Client *client = _clients[_fd[i].fd];

				if (client->getReadyToSend() == true)
					sendResponse(client, _fd[i]);
			}
		}
	}
	close(socketServer);
}
