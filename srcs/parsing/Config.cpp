#include "Config.hpp"
#include <iostream>
// Default constructor
Config::Config()
{
	
}

int isDigits(std::string str)
{
	for (int i = 0; (size_t)i < str.length(); i++) {
		if (!(str[i] >= 48 && str[i] <= 57)) {
			return false;
		}
	}
	return true;
}

int getContent(std::string s)
{
	int i = 0;

	while (!std::isspace(s[i]))
		i++;
	return (i);
}

void Config::setErrorPage(std::string s)
{
	std::string token;
	std::vector<unsigned int> lst;
  	std::vector<unsigned int>::iterator it;
	std::string path;

	it = lst.begin();
	while (s.length() != 0)
	{
		token = s.substr(0, getContent(s));
		s.erase(0, token.length() + 1);
		if (isDigits(token))
			lst.push_back(atoi(token.c_str()));
		if (!isDigits(token))
		{
			token.erase(token.length() - 1, token.length());
			path = token;
		}
	}
	for (std::vector<unsigned int>::iterator vit = lst.begin(); vit != lst.end(); ++vit)
		this->_errorPage.insert(std::make_pair(*vit, path));
}

void Config::setMaxSize(std::string s)
{
	s.erase(s.length() - 1, s.length());
	if (isDigits(s))
		this->_maxSize = std::atoi(s.c_str());
}

void Config::setHostname(std::string s)
{
	s.erase(s.length() - 1, s.length());
	this->_hostname.push_back(s);
}

void Config::setListen(std::string s)
{
	s.erase(s.length() - 1, s.length());
	this->_listen.push_back(s);
}

void Config::setSite(Site s)
{
	this->_site.push_back(s);
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
	for (std::vector<std::string>::iterator it = this->_listen.begin();
		it != this->_listen.end();
		++it)
	{
		std::cout << *it << std::endl;
	}
}

void Config::printMaxSize()
{
	std::cout << "MaxSize : " << this->_maxSize << std::endl;
}

void Config::printData()
{
	this->printHostname();
	this->printListen();
	this->printErrorPage();
	this->printMaxSize();
}
