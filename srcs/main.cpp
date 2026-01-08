#include <header.hpp>

int getContent(std::string s)
{
	int i = 0;

	while (!std::isspace(s[i]))
		i++;
	return (i);
}

int parseLocation(Location *location, std::string s)
{
	std::string del = " ";
	std::string title;

	if (!haveSemiColon(s))
	{
		std::cout << RED << "Error: missing semicolon";
		return (1);
	}
	s = delSemiColon(s);
	title = s.substr(0, s.find(del));
	s.erase(0, title.length() + 1);

	if (title == "methods")
	{
		if (checkOnlySpace(s))
		{
			std::cout << RED << "Error: method list empty";
			return (1);
		}
		std::string method;
		while (method.length() != s.length() || method != s)
		{
			method = s.substr(0, s.find(del));
			if (checkMethod(method))
			{
				std::cout << RED << "Error: methods invalid (GET/POST/DELETE) ";
				return (1);
			}
			s.erase(0, s.find(del) + 1);
			if(checkContent(location->getMethods(), method))
			{
				std::cout << RED << "Error: double methods";
				return (1);
			}

			location->setMethod(method);
		}
		if (location->getMethods().empty())
		{
			std::cout << RED << "Error: method list empty";
			return (1);
		}
	}
	else if (title == "listDirectory")
	{
		if (checkBoolean(s))
		{
			std::cout << RED << "Error: boolean incorrect (true/false)";
			return (1);
		}
		bool b;

		//TODO check if is true or false with no caps
		std::istringstream(s) >> std::boolalpha >> b;
		location->setListDirectory(b);
		
	}
	else if (title == "defaultFile")
	{
		if (s.empty())
		{
			std::cout << RED << "Error: empty path";
			return (1);
		}
		if (s.find(" ") < s.length())
		{
			std::cout << RED << "Error: space in path";
			return (1);
		}
		location->setDefaultFile(s);
	}
	else if (title == "uploadingFile")
	{
		if (checkBoolean(s))
		{
			std::cout << RED << "Error: boolean incorrect (true/false) ";
			return (1);
		}
		bool b;
		//TODO check if is true or false with no caps
		std::istringstream(s) >> std::boolalpha >> b;
		location->setUploadingFile(b);
	}
	else if (title == "root")
	{
		if (s.empty())
		{
			std::cout << RED << "Error: empty path";
			return (1);
		}
		if (s.find(" ") < s.length())
		{
			std::cout << RED << "Error: space in path";
			return (1);
		}
		location->setRoot(s);
	}
	else if (title == "redirection")
	{
		std::string redir;
		while (redir.length() != s.length() || redir != s)
		{
			redir = s.substr(0, s.find(del));
			s.erase(0, s.find(del) + 1);
			location->setRedirection(redir);
		}
	}
	else if (title == "CGI")
	{
		if (s.empty())
		{
			std::cout << RED << "Error: empty path";
			return (1);
		}
		if (s.find(" ") < s.length())
		{
			std::cout << RED << "Error: space in path";
			return (1);
		}
		location->setCGI(s);
	}
	else
	{
		std::cout << RED << "Error: This directive '" << title << "' does not exist in the Location configuration";
		return (1);
	}

	return 0;
}

int parseConfig(Server *serv, std::string s)
{
	std::string del = " ";
	std::string title;

	if (!haveSemiColon(s))
	{
		std::cout << RED << "Error: missing semicolon";
		return (1);
	}
	s = delSemiColon(s);
	title = s.substr(0, s.find(del));
	s.erase(0, title.length() + 1);

	//TODO error message ?
	if (title == "errorPage")
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
			{
				if (checkHTTPCode(std::atoi(token.c_str())))
				{
					std::cout << RED << "Error: invalid error code '" << token << "'";
					return (1);
				}
				lst.push_back(atoi(token.c_str()));
			}
			if (!isDigits(token))
			{
				token.erase(token.length() - 1, token.length());
				path = token;
			}
		}
		if (lst.empty())
		{
			std::cout << RED << "Error: error code list empty";
			return (1);
		}
		if (path.empty())
		{
			std::cout << RED << "Error: error path is empty";
			return (1);
		}
		for (std::vector<unsigned int>::iterator vit = lst.begin(); vit != lst.end(); ++vit)
			serv->setErrorPage(*vit, path);
	}
	else if (title == "maxSize")
	{
		if (serv->getMaxSize() != 0)
		{
			std::cout << RED << "Error: double directive maxSize";
			return (1);
		}
		if (!isDigits(s))
		{
			if (std::atoi(s.c_str()) < 0)
			{
				std::cout << RED << "Error: maxSize negativ";
				return (1);
			}
			std::cout << RED << "Error: invalid char";
			return (1);
		}
		if (s.empty())
		{
			std::cout << RED << "Error: maxSize null";
			return (1);
		}
		
		serv->setMaxSize(std::atoi(s.c_str()));
	}
	else if (title == "hostname")
	{
		if (s.empty())
		{
			std::cout << RED << "Error: hostname null";
			return (1);
		}
		serv->setHostname(s);
	}
	else if (title == "listen")
	{
		if (!(s.find(":") < s.length()))
		{
			std::cout << RED << "Error: colon missing";
			return (1);
		}
		ListenUrl lu;
		lu.host = s.substr(0, s.find(":"));
		s.erase(0, lu.host.length() + 1);
		if (!checkOnlyNumber(s))
		{
			std::cout << RED << "Error: port isn't a number";
			return (1);
		}
		lu.port = std::atoi(s.c_str());

		if (lu.host.empty())
			lu.host = "0.0.0.0";
		if (lu.port == 0)
			lu.port = 80;
		if (!(lu.port >= 1 && lu.port <= 65535))
		{
			std::cout << RED << "Error: port out of bounds (1-65535)";
			return (1);
		}
		if (checkDoubleListen(serv->getListen(), lu))
		{
			std::cout << RED << "Error: duplicate listen";
			return (1);
		}
		serv->setListen(lu);
	}
	else
	{
		std::cout << RED << "Error: This directive '" << title << "' does not exist in the server configuration";
		return (1);
	}
	return (0);
}

int parse(std::vector<Server> *serv, std::string filepath)
{
	std::ifstream file;
	std::string s;
	int line = 0;
	bool serverConf = false;
	bool locationConf = false;

	file.open(filepath.c_str());
	while (std::getline(file, s))
	{
		line++;
		//std::cout << s << std::endl;
		std::cout << YELLOW << "server config : " << serverConf << RESET << std::endl;
		if (s.find("server") < s.length() && !serverConf)
		{
			if (haveSemiColon(s))
			{
				std::cout << RED << "Error: semicolon present after open embrace at line " << line << RESET << std::endl;
				return (1);
			}
			serverConf = true;
			//std::cout << "========SERVER DATA========" << std::endl; 
			Server server;
			while (std::getline(file, s))
			{
				line++;
				if (s.find("server") < s.length() || !serverConf)
				{
					std::cout << RED << "Error: missing accolade at line " << line << RESET << std::endl;
					return (1);
				}
				
				if (s.find("}") < s.length())
				{
					if (checkEmbrace(s))
					{
						std::cout << RED << "Error: character after closed embrace at line " << line << RESET << std::endl;
						return (1);
					}
					serv->push_back(server);
					serverConf = false;
					break;
				}
				
				s = delWhiteSpace(s);
				if (s.find("{") < s.length() && !locationConf)
				{
					if (haveSemiColon(s))
					{
						std::cout << RED << "Error: semicolon present after open embrace at line " << line << RESET << std::endl;
						return (1);
					}
					locationConf = true;
					Location location;
					//TODO de la merde voir mieux
					std::string name = s.substr(0, s.find("{"));
					name = name.substr(0, name.find(" "));
					if (name.empty())
					{
						std::cout << RED << "Error: missing Location name at line " << line << RESET << std::endl;
						return (1);
					}
					location.setName(name);
					std::cout << YELLOW << "Location config : " << locationConf << RESET << std::endl;
					//std::cout << "========Location DATA========" << std::endl;
					while (std::getline(file, s))
					{
						line++;
						//std::cout << s << std::endl;
						if (s.find("{") < s.length() || !locationConf)
						{
							std::cout << RED << "Error: missing accolade at line " << line << RESET << std::endl;
							return (1);
						}
						
						s = delWhiteSpace(s);
						if (s.find("}") < s.length())
						{
							if (checkEmbrace(s))
							{
								std::cout << RED << "Error: character after closed embrace at line " << line << RESET << std::endl;
								return (1);
							}
							server.setLocation(location);
							locationConf = false;
							break;
						}
						else
						{
							if (parseLocation(&location, s))
							{
								std::cout << " at line " << line << RESET << std::endl;
								return (1);
							}
						}
					}
				}
				else
				{
					if (parseConfig(&server, s))
					{
						std::cout << " at line " << line << RESET << std::endl;
						return (1);
					}
				}
			}
		}
		else
		{
			std::cout << RED << "Error: directive on root at line " << line << RESET << std::endl;
			return (1);
		}
	}
	if (serverConf || locationConf)
	{
		std::cout << RED << "Error: missing accolade at line " << line << RESET << std::endl;
		return (1);
	}
	return (0);
}

int main(int ac, char **av)
{
	std::vector<Server> serv;
	bool conf = false;
	std::string filepath = av[ac - 1];

	std::ifstream f(filepath.c_str());
	if (f.good())
	{
		if (parse(&serv, filepath) == 1)
		{
			return (1);
		}

		if (conf)
		{
			for (std::vector<Server>::iterator it = serv.begin();
				it != serv.end();
				++it)
			{
				it->printData();
			}
		}
		return (0);
	}
	std::cout << RED << "Error: file not exist" << RESET << std::endl;
	return (1);
}
