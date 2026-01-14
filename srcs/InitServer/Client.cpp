#include "Client.hpp"

Client::Client(int fd)
{
	_id = fd;
	_readyToSend = false;
	_byteSend = 0;
	_lastTime = time(NULL);

	// provisoire ce que je met dedans
	_response = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 50\n\n<h1>bonjour ok voila</h1>";
}

Client::Client(const Client& other)
{
	_id = other._id;
	_request = other._request;
	_response = other._response;
	_readyToSend = other._readyToSend;
	_byteSend = other._byteSend;
}

Client& Client::operator=(const Client& other)
{
	if (this != &other)
	{
		_id = other._id;
		_request = other._request;
		_response = other._response;
		_readyToSend = other._readyToSend;
		_byteSend = other._byteSend;
	}
	return (*this);
}

Request& Client::getRequest()
{
	return _request;
}

Client::~Client()
{
	_readyToSend = false;
}

void Client::processRequest(const char *buffer, int size)
{
    setLastTime(time(NULL));

	// DEBUG
	// std::cout << "Message recu : " << buffer << std::endl;
	// std::cout << "Message de id : " << client->getID() << " dans _client : " << client->getRequest() << std::endl;
	// std::cout << "Octet recu : " << ret << " | Total octet : " << client->getRequest().size() << std::endl;
	// DEBUG

	bool success = _request.parse(buffer, size);

	if (success == false)
	{
		// mettre la page d erreur direct avec fonction
		std::cout << "Parsing error : " << _request.getErrorCode() << std::endl;
		_readyToSend = true;
		return ;
	}

	if (_request.isComplete())
	{
		// DEBUG
		std::cout << "Requete complete recu !" << std::endl;
		std::cout << "Methode : " << _request.getMethod() << std::endl;
		std::cout << "Taille du body : " << _request.getBody().size() << std::endl;
		// DEBUG

		_readyToSend = true;
	}
	else
	{
		std::cout << "requete pas encre recu entierement.." << std::endl;
	}

}

void Client::setLastTime(time_t time)
{
	_lastTime = time;
}

int Client::getID() const
{
	return _id;
}


std::string& Client::getResponse()
{
	return _response;
}

bool& Client::getReadyToSend()
{
	return _readyToSend;
}

unsigned long& Client::getByteSend()
{
	return _byteSend;
}

time_t& Client::getLastTime()
{
	return _lastTime;
}
