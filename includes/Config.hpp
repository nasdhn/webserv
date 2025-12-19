#ifndef PARSING_HPP
# define PARSING_HPP

# include <string>
# include <map>
# include <vector>
# include <algorithm>
# include "Site.hpp"

class Config
{
	private:
		int _maxSize;
		std::map<unsigned int, std::string> _errorPage;
		std::vector<std::string> _hostname;
		std::vector<std::string> _listen;
		std::vector<Site> _site;

	public:
		Config();


		void setErrorPage(std::string s);
		void setMaxSize(std::string s);
		void setHostname(std::string s);
		void setListen(std::string s);
		void setSite(Site s);


		void printErrorPage();
		void printHostname();
		void printListen();
		void printMaxSize();
		void printSite();
		void printData();
};

#endif