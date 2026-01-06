#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <list>
#include <sstream>
#include <cctype>
#include <clocale>

#include <../colors.hpp>
#include <Config.hpp>

std::string delWhiteSpace(std::string s)
{
	int i = 0;
	while (std::isspace(s[i]))
		i++;
	s.erase(0, i);
	return s;
}

std::string delSemiColon(std::string s)
{
	if (s.find(";") <= s.length())
	{
		int pos = s.find(";");
		s.erase(pos, pos + 1);
	}
	return (s);
}

int parse_site(Site *site, std::string s)
{
	std::string del = " ";
	std::string title;

	s = delSemiColon(s);
	title = s.substr(0, s.find(del));
	s.erase(0, title.length() + 1);

	if (title == "methods")
	{
		std::string method;
		while (method.length() != s.length() || method != s)
		{
			method = s.substr(0, s.find(del));
			s.erase(0, s.find(del) + 1);
			site->setMethod(method);
		}
	}
	else if (title == "listDirectory")
	{
		bool b;
		//TODO check if is true or false with no caps
		std::istringstream(s) >> std::boolalpha >> b;
		site->setListDirectory(b);
	}
	else if (title == "defaultFile")
	{
		if (s.find(" ") > s.length())
			site->setDefaultFile(s);
	}
	else if (title == "uploadingFile")
	{
		bool b;
		//TODO check if is true or false with no caps
		std::istringstream(s) >> std::boolalpha >> b;
		site->setUploadingFile(b);
	}
	else if (title == "root")
	{
		if (s.find(" ") > s.length())
			site->setRoot(s);
	}
	else if (title == "redirection")
	{
		std::string redir;
		while (redir.length() != s.length() || redir != s)
		{
			redir = s.substr(0, s.find(del));
			s.erase(0, s.find(del) + 1);
			site->setRedirection(redir);
		}
	}
	else if (title == "CGI")
	{
		if (s.find(" ") > s.length())
			site->setCGI(s);
	}
	else
	{
		std::cout << RED << "Error: This directive '" << title << "' does not exist in the site configuration" << RESET << std::endl;
		return (1);
	}

	return 0;
}

int parse_config(Config *conf, std::string s)
{
	std::string del = " ";
	std::string title;

	s = delSemiColon(s);
	title = s.substr(0, s.find(del));
	s.erase(0, title.length() + 1);
	//std::cout << title << std::endl;

	//TODO find all params mandatory
	//TODO error message ?

	//TODO server part
	if (title == "errorPage")
	{
		conf->setErrorPage(s);
	}
	else if (title == "maxSize")
	{
		conf->setMaxSize(s);
	}
	else if (title == "hostname")
	{
		conf->setHostname(s);
	}
	else if (title == "listen")
	{
		conf->setListen(s);
	}
	else
	{
		std::cout << RED << "Error: This directive '" << title << "' does not exist in the server configuration" << RESET << std::endl;
		return (1);
	}
	return (0);
}

int parse(std::vector<Config> *serv, int ac, char **av)
{
	std::ifstream file;
	std::string s;
	bool server_conf = false;
	bool site_conf = false;

	file.open(av[ac - 1]);
	while (std::getline(file, s))
	{
		//std::cout << s << std::endl;
		std::cout << YELLOW << "server config : " << server_conf << RESET << std::endl;
		if (s.find("server") < s.length() && !server_conf)
		{
			server_conf = true;
			//std::cout << "========SERVER DATA========" << std::endl; 
			Config conf;
			while (std::getline(file, s))
			{
				if (s.find("server") < s.length() || !server_conf)
				{
					std::cout << RED << "Error: missing accolade" << RESET << std::endl;
					return (1);
				}
				if (s.find("}") < s.length())
				{
					serv->push_back(conf);
					server_conf = false;
					break;
				}
				
				s = delWhiteSpace(s);
				if (s.find("{") < s.length() && !site_conf)
				{
					site_conf = true;
					Site site;
					//TODO de la merde voir mieux
					std::string name = s.substr(0, s.find("{"));
					name = name.substr(0, name.find(" "));

					site.setName(name);
					std::cout << YELLOW << "site config : " << site_conf << RESET << std::endl;
					//std::cout << "========SITE DATA========" << std::endl;
					while (std::getline(file, s))
					{
						//std::cout << s << std::endl;
						if (s.find("{") < s.length() || !site_conf)
						{
							std::cout << RED << "Error: missing accolade" << RESET << std::endl;
							return (1);
						}
						
						s = delWhiteSpace(s);
						if (s.find("}") < s.length())
						{
							conf.setSite(site);
							site_conf = false;
							break;
						}
						else
							if (parse_site(&site, s))
								return (1);
					}
				}
				else
					if (parse_config(&conf, s))
						return (1);
			}
		}
	}
	return (0);
}

int main(int ac, char **av)
{
	std::vector<Config> serv;
	bool conf = false;

	if (parse(&serv, ac, av) == 1)
	{
		return (1);
	}

	if (conf)
	{
		for (std::vector<Config>::iterator it = serv.begin();
			it != serv.end();
			++it)
		{
			it->printData();
		}
	}
	return (0);
}
