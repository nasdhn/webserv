#include "Server.hpp"

volatile bool sigPressed = true; 

Server::Server()
{
	// pour tester avant d ajouter via le parsing
	_port.push_back(8080);
    _port.push_back(8180);
    _port.push_back(9090);
}

Server::Server(const Server &other)
{
	_fd = other._fd;
	_clients = other._clients;
	_readyToSend = other._readyToSend;
	_port = other._port;
}
Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		_fd = other._fd;
		_clients = other._clients;
		_readyToSend = other._readyToSend;
		_port = other._port;
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

void signalHandler(int sig)
{
	(void)sig;
	sigPressed = false;
}

bool Server::isServerSocket(int fd)
{
	for (unsigned int i = 0; i < _serverSockets.size(); i++)
	{
		if (_serverSockets[i] == fd)
			return true;
	}
	return false;
}

void Server::checkTimeOut()
{
	std::vector<int> toDelete;
	time_t curTime; 

	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		curTime = time(NULL);

		if ((curTime - it->second->getLastTime()) > 60) // le 60 sera changer par la valeur parser du .conf
			toDelete.push_back(it->first);
	}

	for (unsigned int i = 0; i < toDelete.size(); i++)
	{
		int fd_kill = toDelete[i];
		if (_clients.find(fd_kill) != _clients.end())
		{
			delete _clients[fd_kill];
			_clients.erase(fd_kill);
		}
		close(fd_kill);

		for (unsigned int j = 0; j < _fd.size(); j++)
		{
			if (_fd[j].fd == fd_kill)
			{
				_fd.erase(_fd.begin() + j);
				break;
			}
		}
	}
}

void Server::cleanAll()
{
	// DEBUG
	std::cout << "Arret du serv" << std::endl;
	// DEBUG

	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		delete it->second;
	_clients.clear();

	for (unsigned int i = 0; i < _fd.size(); i++)
	{	
		if (_fd[i].fd >= 0)
			close(_fd[i].fd);
	}
	_fd.clear();
}

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
			client->setLastTime(time(NULL));
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

void Server::servInit(int port)
{
	int socketServer = socket(AF_INET, SOCK_STREAM, 0);
	if (socketServer == -1)
		throw std::runtime_error("Error : Socket's initialisation failed !");
	int opt = 1;
	if (setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		throw std::runtime_error("Error : Set Socket's initialisation failed !");

	struct sockaddr_in address = {};
	address.sin_family = AF_INET;
	address.sin_port = htons(port); // a completer avec parsing
	address.sin_addr.s_addr = INADDR_ANY;
	// address.sin_zero[(sizeof(address))];

	int flag = fcntl(socketServer, F_GETFL, 0); // voir pour changer getfl
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
	_serverSockets.push_back(socketServer);
}

void Server::setupServ()
{

	for (unsigned long i = 0; i < _port.size(); i++)
		servInit(_port[i]);

	// int socketServer = servInit();

	struct sockaddr_in clientAddr = {};
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(8080); // a completer avec parsing
	clientAddr.sin_addr.s_addr = INADDR_ANY;
	// address.sin_zero[(sizeof(address))];
	socklen_t sizeaddr = sizeof(clientAddr);

	signal(SIGINT, signalHandler);

	while (sigPressed)
	{
		checkTimeOut();
		// integration de poll

		int pollret = poll(&_fd[0], _fd.size(), 1000);
		if (pollret == -1)
		{	
			if (errno == EINTR)
				break;
			throw std::runtime_error("Error : Poll's initialisation failed !");
		}
		if (pollret == 0)
			continue;

		for (unsigned int i = 0; i < _fd.size(); i++)
		{
			if (_fd[i].revents & POLLIN)
			{
				// a chaque client on passe la dedans
				if (isServerSocket(_fd[i].fd))
				{
					int clientSocket = accept(_fd[i].fd, (struct sockaddr *)&clientAddr, &sizeaddr); // a changer pour mettre le nom dune struct client avec info parsing
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
						client->setLastTime(time(NULL));
						buffer[ret] = '\0';
						client->getRequest().append(buffer, ret);

						// DEBUG
						// std::cout << "Message recu : " << buffer << std::endl;
						// std::cout << "Message de id : " << client->getID() << " dans _client : " << client->getRequest() << std::endl;
						// std::cout << "Octet recu : " << ret << " | Total octet : " << client->getRequest().size() << std::endl;
						// DEBUG

						// le parsing ne gere pas les gros fichiers c est seulement temporaires, pas oublier de faire les modifs qund je pourrais
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
									std::cout << "Body pas encore tout lu" << std::endl; // DEBUG
									continue;
								}
								else
								{
									client->getReadyToSend() = true;
									std::cout << "body entierement lu" << std::endl;// DEBUG
								}
							}
							else
							{
								// pas de content lenght donc direct passer a la reponse
								client->getReadyToSend() = true;
								std::cout << "pas de content leghnt" << std::endl;// DEBUG
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
	cleanAll();
}
