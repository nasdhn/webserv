#ifndef SERVER_HPP
# define SERVER_HPP

# include "header.hpp"
# include "Location.hpp"



//listen
//server_name
//root
//index ?
//error_page
//client_max_body_size
//location
//cgi_handle?
//autoindex ?

class Server
{
	private:
		std::string _serverName;
		std::string _root;
		std::string _index;
		bool _autoIndex;
		unsigned int _maxSize;
		std::map<unsigned int, std::string> _errorPage;
		std::vector<std::string> _hostname; //?
		std::vector<listenUrl> _listen;
		std::vector<Location> _location;

	public:
		Server();

		void setErrorPage(int ep, std::string path);
		void setMaxSize(unsigned int size);
		void setHostname(std::string s);
		void setListen(listenUrl s);
		void setLocation(Location s);
		void setServerName(std::string name);
		void setRoot(std::string s);
		void setIndex(std::string s);
		void setAutoIndex(bool s);


		std::string getServerName();
		std::string getRoot();
		unsigned int getMaxSize();
		std::vector<listenUrl> getListen();

		void printErrorPage();
		void printHostname();
		void printListen();
		void printMaxSize();
		void printLocation();
		void printData();
};

#endif