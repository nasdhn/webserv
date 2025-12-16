#include "Client.hpp"

Client::Client(int fd)
{
	_id = fd;
}

Client::Client(const Client& other)
{
	_id = other._id;
	_request = other._request;
	_header = other._header;
}

Client& Client::operator=(const Client& other)
{
	if (this != &other)
	{
		_id = other._id;
		_request = other._request;
		_header = other._header;
	}
	return (*this);
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

Client::~Client()
{

}