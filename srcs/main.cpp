#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <list>

#include <Config.hpp>

int parse(Config *conf, std::string s)
{
	std::string del = " ";
	std::string title;
	(void)conf;

	int i = 0;
	while (std::isspace(s[i]))
	{
		s.erase(0, i + 1);
		i++;
	}
	title = s.substr(0, s.find(del));
	std::cout << title << std::endl;

	//TODO find all params mandatory
	//TODO error message ?

	//TODO server part
	if (title == "errorPage")
	{
		s.erase(0, title.length() + 1);
		conf->setErrorPage(s);
	}
	else if (title == "maxSize")
	{
		s.erase(0, title.length() + 1);
		conf->setMaxSize(s);
	}
	else if (title == "hostname")
	{
		s.erase(0, title.length() + 1);
		conf->setHostname(s);
	}
	else if (title == "listen")
	{
		s.erase(0, title.length() + 1);
		conf->setListen(s);
	}

	return (0);
}

int main(int ac, char **av)
{
	std::ifstream file;
	std::string line;
	std::string delimiter = " ";
	std::string token;
	Config conf;

	file.open(av[ac - 1]);
	std::getline(file, line);
	//std::cout << !line.find("server") << std::endl;
	if (!line.find("server"))
	{
		while (std::getline(file, line))
		{
			parse(&conf, line);
		}
	}
	
	conf.printErrorPage();
}
