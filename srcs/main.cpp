#include <header.hpp>

int getContent(std::string s)
{
	int i = 0;

	while (i < (int)s.length() && !std::isspace(s[i]))
		i++;
	return (i);
}

void delComment(std::string &s)
{
	size_t pos = s.find('#');
	if (pos != std::string::npos)
		s.erase(pos);
	
	if (!s.empty() && s[s.length() - 1] == ' ')
		s.erase(s.length() - 1);
}

int getNbParams(std::string s)
{
	size_t i = 0;
	int p = 0;

	if (std::isprint(s[i]))
		p++;
	while (i < s.length())
	{
		if ((std::isspace(s[i]) && !std::isspace(s[i + 1])))
			p++;
		i++;
	}
	return (p);
}

int parseLocation(Location *location, std::string s)
{
	std::string del = " ";
	std::string title;

	/*if (!haveSemiColon(s))
	{
		std::cout << RED << "Error: missing semicolon";
		return (1);
	}*/
	s = delSemiColon(s);
	title = s.substr(0, s.find(del));
	s.erase(0, title.length() + 1);

	if (title == "allow_methods")
	{
		if (checkOnlySpace(s))
		{
			std::cout << RED << "Error: method list empty";
			return (1);
		}
		std::string method;
		while (s.length() > 0)
		{
			method = s.substr(0, s.find(del));
			if (checkMethod(method))
			{
				std::cout << RED << "Error: methods invalid (GET/POST/DELETE) ";
				return (1);
			}
			s.erase(0, method.length() + 1);
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
	else if (title == "autoindex")
	{
		if (location->getAutoIndex() != 2)
		{
			std::cout << RED << "Error: double autoindex directive";
			return (1);
		}
		if (s != "on" && s != "off")
		{
			std::cout << RED << "Error: boolean incorrect (on/off)";
			return (1);
		}
		bool b = false;
		if (s == "on")
			b = true;
		else if (s == "off")
			b = false;
		location->setListDirectory(b);
		
	}
	else if (title == "index")
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
		location->setIndex(s);
	}
	else if (title == "upload_path")
	{
		if (!location->getUploadPath().empty())
		{
			std::cout << RED << "Error: double upload_path directive";
			return (1);
		}
		if (getNbParams(s) > 1)
		{
			std::cout << RED << "Error: too much params";
			return (1);
		}
		if (s.empty())
		{
			std::cout << RED << "Error: upload path empty";
			return (1);
		}
		location->setUploadPath(s);
	}
	else if (title == "return")
	{
		if (location->getRedir().code != 0)
		{
			std::cout << RED << "Error: double return directive";
			return (1);
		}
		redir r;
		if (getNbParams(s) < 1 || getNbParams(s) > 2)
		{
			std::cout << RED << "Error: too much or not enough params";
			return (1);
		}
		if (!checkOnlyNumber(s.substr(0, s.find(" "))))
		{
			std::cout << RED << "Error: code need to be first";
			return (1);
		}
		r.code = std::atoi(s.substr(0, s.find(" ")).c_str());
		s.erase(0, s.find(" ") + 1);
		r.link = s;
		location->setRedirection(r);
	}
	else if (title == "cgi")
	{
		if (getNbParams(s) != 2)
		{
			std::cout << RED << "Error: needed 2 params in cgi";
			return (1);
		}
		cgi c;
		c.ext = s.substr(0, s.find(" "));
		if (c.ext.empty())
		{
			std::cout << RED << "Error: extension path empty";
			return (1);
		}
		if (c.ext[0] != '.')
		{
			std::cout << RED << "Error: extension need to start with '.'";
			return (1);
		}
		s.erase(0, c.ext.length() + 1);
		c.path = s.substr(0, s.find(" "));
		if (c.path.empty())
		{
			std::cout << RED << "Error: empty path";
			return (1);
		}
		if (c.path.find(" ") < s.length())
		{
			std::cout << RED << "Error: space in path";
			return (1);
		}
		if(checkDoubleCgi(location->getCgi(), c))
		{
			std::cout << RED << "Error: double cgi";
			return (1);
		}
			
		location->setCGI(c);
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

	/*if (!haveSemiColon(s))
	{
		std::cout << RED << "Error: missing semicolon";
		return (1);
	}*/
	s = delSemiColon(s);
	title = s.substr(0, s.find(del));
	s.erase(0, title.length() + 1);

	//TODO error message ?
	if (title == "error_page")
	{
		if (s.empty())
		{
			std::cout << RED << "Error: params empty";
			return (1);
		}
		if (getNbParams(s) < 2)
		{
			std::cout << RED << "Error: not enough params";
			return (1);
		}
		std::string token;
		std::vector<unsigned int> lst;
		std::vector<unsigned int>::iterator it;
		std::string path;
		bool isLink = false;

		it = lst.begin();
		while (s.length() != 0)
		{
			token = s.substr(0, getContent(s));
			s.erase(0, token.length() + 1);
			if (isDigits(token))
			{
				if (isLink)
				{
					std::cout << RED << "Error: error code after link";
					return (1);
				}
				if (checkHTTPCode(std::atoi(token.c_str())))
				{
					std::cout << RED << "Error: invalid error code '" << token << "'";
					return (1);
				}
				lst.push_back(atoi(token.c_str()));
			}
			if (!isDigits(token))
			{
				path = token;
				isLink = true;
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
	else if (title == "client_max_body_size")
	{
		//TODO can have char ?
		if (serv->getMaxSize() != 0)
		{
			std::cout << RED << "Error: double directive client_max_body_size";
			return (1);
		}
		if (s.empty())
		{
			std::cout << RED << "Error: maxSize null";
			return (1);
		}
		if (getNbParams(s) > 1)
		{
			std::cout << RED << "Error: too much params";
			return (1);
		}
		unsigned long long size = 0;
		if (s.find("m") < s.length() || s.find("M") < s.length())
		{
			s.erase(s.length() - 1);
			size = std::atoi(s.c_str());
			size *= 1024 * 1024;
		}
		else if (s.find("k") < s.length() || s.find("K") < s.length())
		{
			s.erase(s.length() - 1);
			size = std::atoi(s.c_str());
			size *= 1024;
		}
		else
		{
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
		}
		
		serv->setMaxSize(size);
	}
	else if (title == "listen")
	{
		if (!(s.find(":") < s.length()))
		{
			std::cout << RED << "Error: colon missing";
			return (1);
		}
		if (getNbParams(s) > 1)
		{
			std::cout << RED << "Error: too many params";
			return (1);
		}
		listenUrl lu;
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
	else if (title == "server_name")
	{
		//TODO can be multiple
		if (s.empty())
		{
			std::cout << RED << "Error: server_name null";
			return (1);
		}
		std::string name;
		while (name.length() != s.length() || name != s)
		{
			name = s.substr(0, s.find(del));
			s.erase(0, s.find(del) + 1);
			if(checkContent(serv->getServerName(), name))
			{
				std::cout << RED << "Error: same server name";
				return (1);
			}

			serv->setServerName(name);
		}
	}
	else if (title == "root")
	{
		if (!serv->getRoot().empty())
		{
			std::cout << RED << "Error: double directive root";
			return (1);
		}
		if (s.empty())
		{
			std::cout << RED << "Error: root path null";
			return (1);
		}
		if (getNbParams(s) > 1)
		{
			std::cout << RED << "Error: too many params";
			return (1);
		}
		serv->setRoot(s);
	}
	else if (title == "index")
    {
        if (s.empty())
        {
            std::cout << RED << "Error: index path empty";
            return (1);
        }
        serv->clearIndex();
        std::string filename;
        std::string del = " ";
        
        while (!s.empty())
        {
            size_t pos = s.find(del);
            if (pos != std::string::npos)
            {
                filename = s.substr(0, pos);
                s.erase(0, pos + 1);
            }
            else
            {
                filename = s;
                s.clear();
            }
            if (!filename.empty())
                serv->setIndex(filename);
        }
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
		s = delWhiteSpace(s);
		delComment(s);
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
				s = delWhiteSpace(s);
				delComment(s);
				if ((s.find("server") < s.length() && !(s.find("server_name") < s.length())) || !serverConf)
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
				//TODO if # is a start of line after del white space
				if (!(s.empty()))
				{
					s = delWhiteSpace(s);
					if (!(s[0] == '#'))
					{
						if (s.find("location") < s.length() && !locationConf)
						{
							if (haveSemiColon(s))
							{
								std::cout << RED << "Error: semicolon present after open embrace at line " << line << RESET << std::endl;
								return (1);
							}
							locationConf = true;
							Location location;
							//TODO de la merde voir mieux
							s.erase(0, 9);
							std::string name = s.substr(0, s.find(" "));
							if (name.empty())
							{
								std::cout << RED << "Error: missing Location name at line " << line << RESET << std::endl;
								return (1);
							}
							if (name.find("{") < name.length())
							{
								std::cout << RED << "Error: { next to location's name" << line << RESET << std::endl;
								return (1);
							}
							location.setName(name);
							while (std::getline(file, s))
							{
								line++;
								delComment(s);
								s = delWhiteSpace(s);
								if (!(s.empty()))
								{
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

// int main(int ac, char **av)
// {
// 	std::vector<Server> serv;
// 	bool conf = true;
// 	std::string filepath = av[ac - 1];

// 	std::ifstream f(filepath.c_str());
// 	if (f.good())
// 	{
// 		if (parse(&serv, filepath) == 1)
// 		{
// 			return (1);
// 		}

// 		if (conf)
// 		{
// 			for (std::vector<Server>::iterator it = serv.begin();
// 				it != serv.end();
// 				++it)
// 			{
// 				it->printData();
// 			}
// 		}
// 		return (0);
// 	}
// 	std::cout << RED << "Error: file not exist" << RESET << std::endl;
// 	return (1);
// }

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cout << "Usage: ./webserv <config_file>" << std::endl;
        return 1;
    }
    std::vector<Server> serv;
    std::string filepath = av[1];

    std::ifstream f(filepath.c_str());
    if (!f.good())
    {
        std::cout << "Error: file not exist" << std::endl;
        return 1;
    }
    if (parse(&serv, filepath) == 1)
    {
        std::cerr << "Error: Parsing failed" << std::endl;
        return 1;
    }
    try
    {
        WebServ server(serv); 
        server.setupServ();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Server Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}