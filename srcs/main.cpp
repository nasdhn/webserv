#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <list>
#include <sstream>
#include <cctype>
#include <clocale>

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
		else
			std::cout << "MERDE" << std::endl;
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
		else
			std::cout << "MERDE" << std::endl;
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
		else
			std::cout << "MERDE" << std::endl;
	}
	else
	{
		std::cout << "Title : " <<  title << std::endl;
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
	return (0);
}

int parse(std::vector<Config> *serv, int ac, char **av)
{
	std::ifstream file;
	std::string s;

	file.open(av[ac - 1]);
	while (std::getline(file, s))
	{
		//std::cout << s << std::endl;

		if (s.find("{") < s.length())
		{
			//std::cout << "========SERVER DATA========" << std::endl; 
			Config conf;
			while (std::getline(file, s))
			{
				//std::cout << s << std::endl;
				if (s.find("}") < s.length())
				{
					serv->push_back(conf);
					break;
				}
				
				s = delWhiteSpace(s);
				if (s.find("{") < s.length())
				{
					Site site;
					//TODO de la merde voir mieux
					std::string name = s.substr(0, s.find("{"));
					name = name.substr(0, name.find(" "));

					site.setName(name);
					
					//std::cout << "========SITE DATA========" << std::endl;
					while (std::getline(file, s))
					{
						
						//std::cout << s << std::endl;
						s = delWhiteSpace(s);
						if (s.find("}") < s.length())
						{
							//site.printData();
							conf.setSite(site);
							break;
						}
						else
							parse_site(&site, s);
					}
				}
				else
					parse_config(&conf, s);
			}
		}
	}
	return (0);
}

int main(int ac, char **av)
{
	std::vector<Config> serv;

	parse(&serv, ac, av);
	
	for (std::vector<Config>::iterator it = serv.begin();
		it != serv.end();
		++it)
	{
		it->printData();
	}
}
