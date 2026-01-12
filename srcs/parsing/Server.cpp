#include "Server.hpp"
#include <iostream>
// Default constructor
Server::Server()
{
	this->_maxSize = 0;
}

void Server::setErrorPage(int ep, std::string path)
{
	this->_errorPage.insert(std::make_pair(ep, path));
}

void Server::setMaxSize(unsigned int size)
{
	this->_maxSize = size;
}

void Server::setHostname(std::string s)
{
	this->_hostname.push_back(s);
}

void Server::setListen(listenUrl s)
{
	
	this->_listen.push_back(s);
}

void Server::setLocation(Location s)
{
	this->_location.push_back(s);
}

void Server::setServerName(std::string name)
{
	this->_serverName = name;
}

void Server::setRoot(std::string s)
{
	this->_root = s;
}

void Server::setIndex(std::string s)
{
	this->_index = s;
}

void Server::setAutoIndex(bool s)
{
	this->_autoIndex = s;
}

std::string Server::getRoot()
{
	return (this->_root);
}

std::string Server::getServerName()
{
	return (this->_serverName);
}

unsigned int Server::getMaxSize()
{
	return (this->_maxSize);
}

std::vector<listenUrl> Server::getListen()
{
	return (this->_listen);
}

void Server::printErrorPage()
{
	std::cout << "errorPage : " << std::endl;
	for (std::map<unsigned int, std::string>::iterator it = this->_errorPage.begin();
		it != this->_errorPage.end();
		++it)
	{
		std::cout << it->first << " : " << it->second << std::endl;
	}
}

void Server::printHostname()
{
	std::cout << "hostname : " << std::endl;
	for (std::vector<std::string>::iterator it = this->_hostname.begin();
		it != this->_hostname.end();
		++it)
	{
		std::cout << *it << std::endl;
	}
}

void Server::printListen()
{
	std::cout << "listen : " << std::endl;
	for (std::vector<listenUrl>::iterator it = this->_listen.begin();
		it != this->_listen.end();
		++it)
	{
		std::cout << it->host << " " << it->port << std::endl;
	}
}

void Server::printMaxSize()
{
	std::cout << "MaxSize : " << this->_maxSize << std::endl;
}

void Server::printLocation()
{
	std::cout << "Location : " << std::endl;
	for (std::vector<Location>::iterator it = this->_location.begin();
		it != this->_location.end();
		++it)
	{
		it->printData();
	}
}

void Server::printData()
{
	std::cout << "============= Server =============" << std::endl;
	std::cout << "name : " << std::endl << this->_serverName << std::endl;
	this->printHostname();
	this->printListen();
	this->printErrorPage();
	this->printMaxSize();
	this->printLocation();
}
