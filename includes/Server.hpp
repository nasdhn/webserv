#ifndef SERVER_HPP
# define SERVER_HPP

# include "header.hpp"
# include "Location.hpp"

struct listenUrl
{
	std::string host;
	int port;
};

class Server
{
	private:
		std::vector<std::string> _serverName;
		std::string _root;
		unsigned long long _maxSize;
		std::map<unsigned int, std::string> _errorPage;
		std::vector<listenUrl> _listen;
		std::vector<Location> _location;
		std::vector<std::string> _index;

	public:
		Server();

		void setErrorPage(int ep, std::string path);
		void setMaxSize(unsigned long long size);
		void setListen(listenUrl s);
		void setLocation(Location s);
		void setServerName(std::string name);
		void setRoot(std::string s);
		void setIndex(std::string s);

		std::vector<std::string> getServerName();
		std::string getRoot();
		unsigned long long getMaxSize();
		std::vector<listenUrl> getListen();
		std::vector<Location>& getLocation();
		std::map<unsigned int, std::string> getErrorPage();
		std::vector<std::string> getIndex();

		void clearIndex();
		void printErrorPage();
		void printServerName();
		void printListen();
		void printMaxSize();
		void printLocation();
		void printData();
};

#endif