#ifndef SITE_HPP
# define SITE_HPP

# include <string>
# include <map>
# include <vector>

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
		Site();
		~Site();

		void setName(std::string n);

};

#endif