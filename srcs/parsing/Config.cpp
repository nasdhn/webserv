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
			path = token;
	}
	for (std::vector<unsigned int>::iterator vit = lst.begin(); vit != lst.end(); ++vit)
		this->_errorPage.insert(std::make_pair(*vit, path));
}

void Config::printErrorPage()
{
	for (std::map<unsigned int, std::string>::iterator it = this->_errorPage.begin();
		it != this->_errorPage.end();
		++it)
	{
		
			std::cout << "clé = " << it->first     // valeur entière dans la liste
					<< ", valeur = " << it->second
					<< std::endl;
		
	}
}