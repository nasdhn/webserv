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
	_header = other._header;
	_response = other._response;
	_readyToSend = other._readyToSend;
	_byteSend = other._byteSend;


	// provisoire
	_contentLenght_str = other._contentLenght_str;
	_contentSize_int = other._contentSize_int;
}

Client& Client::operator=(const Client& other)
{
	if (this != &other)
	{
		_id = other._id;
		_request = other._request;
		_header = other._header;
		_response = other._response;
		_readyToSend = other._readyToSend;
		_byteSend = other._byteSend;


		// provisoire
		_contentLenght_str = other._contentLenght_str;
		_contentSize_int = other._contentSize_int;
	}
	return (*this);
}

Client::~Client()
{
	_readyToSend = false;
}

void Client::setLastTime(time_t time)
{
	_lastTime = time;
}

int Client::getID() const
{
	return _id;
}

std::string& Client::getRequest()
{
	return _request;
}

std::string& Client::getHeader()
{
	return _header;
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

// fonction provisoire a remplacer ou a suppr par celle de class Request

std::string& Client::getContentSizeString()
{
	return _contentLenght_str;
}

int& Client::getContentSizeInt()
{
	return _contentSize_int;
}