#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <list>

#include <Config.hpp>

std::string delWhiteSpace(std::string s)
{
	int i = 0;
	while (std::isspace(s[i]))
		i++;
	s.erase(0, i);
	return s;
}

int parse(Config *conf, int ac, char **av)
{
	std::ifstream file;
	std::string s;
	std::string del = " ";
	std::string title;

	file.open(av[ac - 1]);
	std::getline(file, s);
	if (!s.find("server"))
	{
		while (std::getline(file, s))
		{
			s = delWhiteSpace(s);
			if (s.find("{") < s.length())
			{
				Site site;
				//TODO de la merde voir mieux
				std::string name = s.substr(0, s.find("{"));
				name = name.substr(0, name.find(" "));
				std::cout << "'" << name << "'"<< std::endl;
				
				site.setName(name);
				while (std::getline(file, s))
				{
					s = delWhiteSpace(s);
					title = s.substr(0, s.find(del));
					std::cout << title << std::endl;
					if (s.find("}") < s.length())
						break;
				}
			}
			else
			{
				title = s.substr(0, s.find(del));
				//std::cout << title << std::endl;

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
			}
		}
	}
	return (0);
}

int main(int ac, char **av)
{
	Config conf;

	parse(&conf, ac, av);
	
	conf.printData();
}
