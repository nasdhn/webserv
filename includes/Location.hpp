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

//allow_methods
//index
//alias
//autoindex
//cgi

//root
//autoindex
//client_max_body_size
//index
//error_page
//cgi_handler
//limit_except
//upload_store
//return

class Location
{
	private:
		std::string _name;
		std::vector<std::string> _methods;
		bool _listDirectory; //can be empty
		std::string _index;
		std::string _uploadPath; //can be empty
		std::string _root;
		redir _redirection;
		std::vector<cgi> _cgi;

	public:
		Location();
		~Location();

		void setName(std::string n);
		void setMethod(std::string s);
		void setListDirectory(bool val);
		void setIndex(std::string s);
		void setUploadPath(std::string s);
		void setRoot(std::string s);
		void setRedirection(redir r);
		void setCGI(cgi c);

		std::vector<std::string> getMethods();

		void printMethods();
		void printListDirectory();
		void printDefaultFile();
		void printUploadingFile();
		void printRoot();
		void printRedirection();
		void printCGI();
		void printData();

};

#endif