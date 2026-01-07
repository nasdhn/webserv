#ifndef PARSING_HPP
# define PARSING_HPP

# include <string>
# include <map>
# include <vector>
# include <algorithm>
# include "Site.hpp"

struct ListenUrl
{
    std::string host;
    int port;
};

class Config
{
	private:
		unsigned int _maxSize;
		std::map<unsigned int, std::string> _errorPage;
		std::vector<std::string> _hostname;
		std::vector<ListenUrl> _listen;
		std::vector<Site> _site;

	public:
		Config();

		void setErrorPage(int ep, std::string path);
		void setMaxSize(unsigned int size);
		void setHostname(std::string s);
		void setListen(ListenUrl s);
		void setSite(Site s);

		unsigned int getMaxSize();

		void printErrorPage();
		void printHostname();
		void printListen();
		void printMaxSize();
		void printSite();
		void printData();
};

#endif