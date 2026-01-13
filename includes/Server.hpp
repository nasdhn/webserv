#ifndef SERVER_HPP
# define SERVER_HPP

# include "header.hpp"
# include "Location.hpp"

class Server
{
	private:
		std::vector<std::string> _serverName;
		std::string _root;
		unsigned int _maxSize;
		std::map<unsigned int, std::string> _errorPage;
		std::vector<listenUrl> _listen;
		std::vector<Location> _location;

	public:
		Server();

		void setErrorPage(int ep, std::string path);
		void setMaxSize(unsigned int size);
		void setListen(listenUrl s);
		void setLocation(Location s);
		void setServerName(std::string name);
		void setRoot(std::string s);

		std::vector<std::string> getServerName();
		std::string getRoot();
		unsigned int getMaxSize();
		std::vector<listenUrl> getListen();

		void printErrorPage();
		void printServerName();
		void printListen();
		void printMaxSize();
		void printLocation();
		void printData();
};

#endif