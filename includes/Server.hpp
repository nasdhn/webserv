#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <map>
# include <vector>
# include <algorithm>
# include "Location.hpp"

struct ListenUrl
{
    std::string host;
    int port;
};

class Server
{
	private:
		unsigned int _maxSize;
		std::map<unsigned int, std::string> _errorPage;
		std::vector<std::string> _hostname;
		std::vector<ListenUrl> _listen;
		std::vector<Location> _location;

	public:
		Server();

		void setErrorPage(int ep, std::string path);
		void setMaxSize(unsigned int size);
		void setHostname(std::string s);
		void setListen(ListenUrl s);
		void setLocation(Location s);

		unsigned int getMaxSize();

		void printErrorPage();
		void printHostname();
		void printListen();
		void printMaxSize();
		void printLocation();
		void printData();
};

#endif