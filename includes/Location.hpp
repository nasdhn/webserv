#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <string>
# include <map>
# include <vector>
# include <iostream>

struct cgi
{
	std::string ext;
	std::string path;
};

struct redir
{
	unsigned int code;
	std::string link;
};

class Location
{
	private:
		std::string _name;
		std::vector<std::string> _methods;
		int _autoIndex;
		std::vector<std::string> _index;
		std::string _uploadPath; //can be empty
		redir _redirection;
		std::vector<cgi> _cgi;
		std::string _root;

	public:
		Location();
		~Location();

		void setName(std::string n);
		void setMethod(std::string s);
		void setListDirectory(bool val);
		void setIndex(std::string s);
		void setUploadPath(std::string s);
		void setRedirection(redir r);
		void setCGI(cgi c);
		void setRoot(std::string s);

		std::vector<std::string> getMethods() const;
		std::string getName() const;
		std::vector<cgi> getCgi();
		int getAutoIndex();
		std::vector<std::string> getIndex();
		std::string getUploadPath();
		redir getRedir();
		std::string getRoot();
		// Dans Location.hpp

		void printMethods();
		void printListDirectory();
		void printDefaultFile();
		void printUploadingFile();
		void printRedirection();
		void printCGI();
		void printData();

};

#endif