#include "Client.hpp"

Client::Client(int fd)
{
	_id = fd;
}

Client::Client(const Client& other)
{
	(void)other;
}

Client& Client::operator=(const Client& other)
{
	(void)other;
	return (*this);
}

Client::~Client()
{

}