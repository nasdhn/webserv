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
		// provisoire
		_contentLenght_str = other._contentLenght_str;
		_contentSize_int = other._contentSize_int;
	}
	return (*this);
}

Client::~Client()
{

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

// fonction provisoire a remplacer ou a suppr par celle de class Request

std::string& Client::getContentSizeString()
{
	return _contentLenght_str;
}

int& Client::getContentSizeInt()
{
	return _contentSize_int;
}