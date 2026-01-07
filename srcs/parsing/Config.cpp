#include "Config.hpp"
#include <iostream>
// Default constructor
Config::Config()
{
	this->_maxSize = 0;
}

void Config::setErrorPage(int ep, std::string path)
{
	this->_errorPage.insert(std::make_pair(ep, path));
}

void Config::setMaxSize(unsigned int size)
{
	this->_maxSize = size;
}

void Config::setHostname(std::string s)
{
	this->_hostname.push_back(s);
}

void Config::setListen(ListenUrl s)
{
	
	this->_listen.push_back(s);
}

void Config::setSite(Site s)
{
	this->_site.push_back(s);
}

unsigned int Config::getMaxSize()
{
	return (this->_maxSize);
}

void Config::printErrorPage()
{
	std::cout << "errorPage : " << std::endl;
	for (std::map<unsigned int, std::string>::iterator it = this->_errorPage.begin();
		it != this->_errorPage.end();
		++it)
	{
		std::cout << it->first << " : " << it->second << std::endl;
	}
}

void Config::printHostname()
{
	std::cout << "hostname : " << std::endl;
	for (std::vector<std::string>::iterator it = this->_hostname.begin();
		it != this->_hostname.end();
		++it)
	{
		std::cout << *it << std::endl;
	}
}

void Config::printListen()
{
	std::cout << "listen : " << std::endl;
	for (std::vector<ListenUrl>::iterator it = this->_listen.begin();
		it != this->_listen.end();
		++it)
	{
		std::cout << it->host << " " << it->port << std::endl;
	}
}

void Config::printMaxSize()
{
	std::cout << "MaxSize : " << this->_maxSize << std::endl;
}

void Config::printSite()
{
	std::cout << "Site : " << std::endl;
	for (std::vector<Site>::iterator it = this->_site.begin();
		it != this->_site.end();
		++it)
	{
		it->printData();
	}
}

void Config::printData()
{
	std::cout << "============= Config =============" << std::endl;
	this->printHostname();
	this->printListen();
	this->printErrorPage();
	this->printMaxSize();
	this->printSite();
}
