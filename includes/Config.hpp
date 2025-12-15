#ifndef PARSING_CPP
# define PARSING_CPP

# include <string>
# include <map>
# include <vector>
# include <algorithm>

class Site
{
	private:
		std::string _name;
		std::vector<std::string> _method;
		bool _listDirectory;
		std::string _defaultFile;
		bool _uploadingFile;
		std::string _root;
		std::map<std::string, std::string> _redirection;
		std::string _cgi;
	public:

};

class Config
{
	private:
		int _maxSize;
		std::map<unsigned int, std::string> _errorPage;
		std::vector<std::string> _hostname; //vector
		std::vector<std::string> _listen; //vector
		//location
			//methods list/vector
			//listDirectory bool
			//defaultFile string
			//uploadingFile bool
			//root string
			//redirection map
			//CGI string

	public:
		Config();


		void setErrorPage(std::string s);
		void setMaxSize(std::string s);
		void setHostname(std::string s);
		void setListen(std::string s);


		void printErrorPage();
};

#endif