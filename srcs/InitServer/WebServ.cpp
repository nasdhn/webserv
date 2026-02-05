#include "WebServ.hpp"
#include "Server.hpp"

volatile bool sigPressed = true;

std::string WebServ::intToStr(int n) 
{
    std::ostringstream ss;
    ss << n;
    return ss.str();
}

void WebServ::printLog(const std::string &msg, const std::string &color)
{
    time_t now = time(0);
    tm *ltm = localtime(&now);

    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", ltm);
    std::cout << "[" << buffer << "] -> " << color << msg << RESET << std::endl;
}

std::vector<Server>& WebServ::getServers() 
{
    return _servers;
}

WebServ::WebServ(std::vector<Server> &serv) : _servers(serv)
{
    std::vector<SocketInfo> socket_open;

    for (size_t i = 0; i < serv.size(); i++)
    {
        for (size_t j = 0; j < serv[i].getListen().size(); j++)
        {
            std::string tmp_ip = serv[i].getListen()[j].host;
            int tmp_port = serv[i].getListen()[j].port;
            bool is_open = false;

            for (size_t p = 0; p < socket_open.size(); p++)
            {
                if (socket_open[p].ip == tmp_ip && socket_open[p].port == tmp_port)
                {
                    is_open = true;
                    break;
                }
            }
            if (is_open == false)
            {
                try 
				{
                    servInit(tmp_ip, tmp_port);
                    
                    SocketInfo info;
                    info.ip = tmp_ip;
                    info.port = tmp_port;
                    socket_open.push_back(info);
                    // DEBUG
					printLog("Listening on " + tmp_ip + ":" + intToStr(tmp_port), GREEN);
					// DEBUG
                }
                catch (std::exception &e) 
				{
					printLog(tmp_ip + ":" + intToStr(tmp_port) + " : " + e.what(), RED);
                }
            }
        }
    }
}

WebServ::WebServ(const WebServ &other)
{
	_fd = other._fd;
	_clients = other._clients;
	_readyToSend = other._readyToSend;
	_serverSockets = other._serverSockets;
}
WebServ &WebServ::operator=(const WebServ &other)
{
	if (this != &other)
	{
		_fd = other._fd;
		_clients = other._clients;
		_readyToSend = other._readyToSend;
		_serverSockets = other._serverSockets;
	}
	return (*this);
}

WebServ::~WebServ()
{
}

std::vector<struct pollfd> WebServ::getFD()
{
	return _fd;
}

void signalHandler(int sig)
{
	(void)sig;
	sigPressed = false;
}

bool WebServ::isServerSocket(int fd)
{
	for (unsigned int i = 0; i < _serverSockets.size(); i++)
	{
		if (_serverSockets[i] == fd)
			return true;
	}
	return false;
}

void WebServ::checkTimeOut()
{
	std::vector<int> toDelete;
	time_t curTime;

	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		curTime = time(NULL);

		if ((curTime - it->second->getLastTime()) > 60)
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
		printLog("\033[1mTimeout du client numéro [" + intToStr(fd_kill) + ']', RED);
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

void WebServ::cleanAll()
{
	printLog("\033[0;35mSERVER SHUTDOWN", BOLD);

	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		delete it->second;
	_clients.clear();
	for (unsigned int i = 0; i < _fd.size(); i++)
	{
		if (_fd[i].fd >= 0)
			close(_fd[i].fd);
	}
	_fd.clear();
}


bool WebServ::sendResponse(Client *client, struct pollfd &pfd)
{
    if (!client->getResponse().empty())
    {
        std::string& msg = client->getResponse();
        int ret = send(pfd.fd, msg.c_str(), msg.size(), 0);

        if (ret > 0)
            msg.erase(0, ret);
        else
            return false;
        if (!msg.empty())
            return true;
    }
	if (client->getResponse().empty() && client->getFileFD() == -1)
    {
        client->getReadyToSend() = false;
        client->getByteSend() = 0;
    	for (size_t k = 0; k < _fd.size(); k++) 
        {
           	if (_fd[k].fd == pfd.fd) 
            {    
                _fd[k].events = POLLIN;
            }
    	}
    	std::string path = client->getRequest().getPath();
    	bool isCgi = (path.find(".py") != std::string::npos || path.find(".php") != std::string::npos || path.find(".cgi") != std::string::npos); 
    	if (isCgi)
        	return false; // pour fermer
    client->reset();
    }
    return true;
}

Server* WebServ::findServer(std::vector<Server>& serv, std::string& host_listen, int port_listen)
{
    Server* defaultServ = NULL;

    std::string hostClean = host_listen;
    size_t pos = hostClean.find(':');
    if (pos != std::string::npos)
        hostClean = hostClean.substr(0, pos);
    for (size_t i = 0; i < serv.size(); i++)
    {
        for (size_t j = 0; j < serv[i].getListen().size(); j++)
        {
            if (serv[i].getListen()[j].port == port_listen)
            {
                if (defaultServ == NULL)
                    defaultServ = &serv[i];
                std::vector<std::string> names = serv[i].getServerName();
                for (size_t k = 0; k < names.size(); k++)
                {
                    if (names[k] == hostClean)
                        return &serv[i];
                }
            }
        }
    }
    return defaultServ;
}


const Location* WebServ::findLocation(Server* server, const std::string& uri)
{
    const Location* match = NULL;
    size_t maxLen = 0;
    const std::vector<Location>& locs = server->getLocation();

    for (size_t i = 0; i < locs.size(); i++)
    {
        const std::string& locName = locs[i].getName();
        if (uri.find(locName) == 0)
        {
            if (locName == "/" || uri.length() == locName.length() || uri[locName.length()] == '/')
            {
                if (locName.length() > maxLen)
                {
                    match = &locs[i];
                    maxLen = locName.length();
                }
            }
        }
    }
    return match;
}

void WebServ::servInit(std::string ip, int port)
{
	int socketServer = socket(AF_INET, SOCK_STREAM, 0);
	if (socketServer == -1)
		throw std::runtime_error("Error : Socket's initialisation failed !");
	int opt = 1;
	if (setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		throw std::runtime_error("Error : Set Socket's initialisation failed !");

	struct sockaddr_in address = {};
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = inet_addr(ip.c_str());
	if (address.sin_addr.s_addr == INADDR_NONE)
	{
    	close(socketServer);
    	throw std::runtime_error("Error : Invalid IP !");
	}	
	// address.sin_zero[(sizeof(address))];
	if (fcntl(socketServer, F_SETFL, O_NONBLOCK) == -1)
	{
		close(socketServer);
		throw std::runtime_error("Error : Set non blocking's initialisation failed !");
	}
	if (bind(socketServer, (struct sockaddr *)&address, sizeof(address)) == -1)
	{
		close(socketServer);
		throw std::runtime_error("Error : Bind failed !");
	}
	if (listen(socketServer, 1024) == -1)
	{
		close(socketServer);
		throw std::runtime_error("Error : Listen failed !");
	}
	// integration de poll
	struct pollfd sfd;
	sfd.fd = socketServer;
	sfd.events = POLLIN;
	sfd.revents = 0;
	_fd.push_back(sfd);
	_serverSockets.push_back(socketServer);
}


void WebServ::setupServ()
{
    struct sockaddr_in clientAddr = {};
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(0);
    clientAddr.sin_addr.s_addr = INADDR_ANY;
    socklen_t sizeaddr = sizeof(clientAddr);

    signal(SIGINT, signalHandler);

    while (sigPressed)
    {
        checkTimeOut();

		// copie de tout les sockets existant : serv, client
        std::vector<struct pollfd> poll_fds = _fd; 

		// ajout des fd de fichiers ou cgi ouvert par le client mais temporairement
        for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            // mode lecture des fichiers ou cgi
            if (it->second->getFileFD() != -1) 
            {
                struct pollfd pfd_file;
                pfd_file.fd = it->second->getFileFD();
                pfd_file.events = POLLIN;
                pfd_file.revents = 0;
                poll_fds.push_back(pfd_file);
            }
            
            // ecriture de l input du cgi pour post
            if (it->second->getCgiInputFD() != -1)
            {
                struct pollfd pfd_cgi_in;
                pfd_cgi_in.fd = it->second->getCgiInputFD();
                pfd_cgi_in.events = POLLOUT;
                pfd_cgi_in.revents = 0;
                poll_fds.push_back(pfd_cgi_in);
            }
        }
		// initialisation de poll
        int pollret = poll(&poll_fds[0], poll_fds.size(), 1000);
        if (pollret == -1) 
		{
            if (errno == EINTR) 
                break;
            throw std::runtime_error("Error : Poll failed !");
        }
        if (pollret == 0) 
			continue;
        for (unsigned int i = 0; i < poll_fds.size(); i++)
        {
            if (poll_fds[i].revents & POLLIN)
            {
				// le cas ou c est un serveur
                if (isServerSocket(poll_fds[i].fd))
                {
                    int clientSocket = accept(poll_fds[i].fd, (struct sockaddr *)&clientAddr, &sizeaddr);
                    if (clientSocket != -1)
                    {
                        fcntl(clientSocket, F_SETFL, O_NONBLOCK);
                        Client *newClient = new Client(clientSocket, this);
                        _clients[clientSocket] = newClient;

                        struct pollfd clientFD;
                        clientFD.fd = clientSocket;
                        clientFD.events = POLLIN;
                        clientFD.revents = 0;
                        _fd.push_back(clientFD); 
                        printLog("Nouveau client connecté [" + intToStr(clientSocket) + ']', BOLD);
                    }
                }
                // le cas ou c'est un iencli
                else if (_clients.find(poll_fds[i].fd) != _clients.end())
                {
                    Client *client = _clients[poll_fds[i].fd];
                    char buffer[BUFFERSIZE] = {0};
                    int ret = recv(poll_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
                    if (ret <= 0) 
                    {
                        printLog("\033[1mDéconnexion du client numéro " + intToStr(poll_fds[i].fd), RED);
                        close(poll_fds[i].fd);
                        delete client;
                        _clients.erase(poll_fds[i].fd);
                        for (size_t k = 0; k < _fd.size(); k++) {
                            if (_fd[k].fd == poll_fds[i].fd) {
                                _fd.erase(_fd.begin() + k);
                                break;
                            }
                        }
                    }
                    else 
                    {
                        client->processRequest(buffer, ret);
                        if (client->getReadyToSend()) 
                        {
                            for (size_t k = 0; k < _fd.size(); k++) 
                            {
                                if (_fd[k].fd == poll_fds[i].fd) 
                                    _fd[k].events = POLLIN | POLLOUT;
                            }
                        }
                    }
                }
                else 
                {
                    Client* owner = NULL;
                    for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) 
                    {
                        if (it->second->getFileFD() == poll_fds[i].fd) 
                        {
                            owner = it->second;
                            break;
                        }
                    }
                    if (owner)
                    {
                        char buffer[BUFFERSIZE];
                        int ret = read(poll_fds[i].fd, buffer, BUFFERSIZE);

                        if (ret > 0)
                        {
                            std::string chunk(buffer, ret);
                            std::string& currentResp = owner->getResponse();
                            size_t cgiHeaderEnd = chunk.find("\r\n\r\n");
      
                            if (cgiHeaderEnd != std::string::npos && chunk.find("Content-Type:") != std::string::npos)
                            {
                                std::string cgiHeaders = chunk.substr(0, cgiHeaderEnd);
                                std::string cgiBody = chunk.substr(cgiHeaderEnd + 4);
                                size_t serverHeaderEnd = currentResp.find("\r\n\r\n");
                                
                                if (serverHeaderEnd != std::string::npos)
                                {
                                    currentResp.insert(serverHeaderEnd, "\r\n" + cgiHeaders);
                                }
                                
                                currentResp.append(cgiBody);
                            }
                            else
                            {
                                currentResp.append(chunk);
                            }
                        }
                        else 
                        {
                            close(poll_fds[i].fd);
                            owner->setFileFD(-1); 
                            owner->getReadyToSend() = true; 
                        }
                    }
                }
            }
            if (poll_fds[i].revents & POLLOUT)
            {
                if (_clients.find(poll_fds[i].fd) != _clients.end())
                {
                    Client *client = _clients[poll_fds[i].fd];
                    if (!client->getResponse().empty() || client->getFileFD() == -1)
                    {
                        if (sendResponse(client, poll_fds[i]) == false)
                        {
                            close(poll_fds[i].fd);
                            delete client;
                            _clients.erase(poll_fds[i].fd);
                            for (size_t k = 0; k < _fd.size(); k++) 
                            {
                                if (_fd[k].fd == poll_fds[i].fd) 
                                {
                                    _fd.erase(_fd.begin() + k);
                                    break;
                                }
                            }
                        }
                    }
                }
                else 
                {
                    Client* ownerCgiIn = NULL;
                    for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) 
					{
                        if (it->second->getCgiInputFD() == poll_fds[i].fd) 
						{
                            ownerCgiIn = it->second;
                            break;
                        }
                    } 
                    if (ownerCgiIn) 
					{
                        ownerCgiIn->handleCgiWrite();
                    }
                }
            }
        }
		int status;
        while (waitpid(-1, &status, WNOHANG) > 0) {		
			}
    }
    cleanAll();
}