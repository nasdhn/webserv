#ifndef SITE_HPP
# define SITE_HPP

# include <string>
# include <map>
# include <vector>
# include <iostream>

class Site
{
	private:
		std::string _name;
		std::vector<std::string> _methods;
		bool _listDirectory; //can be empty
		std::string _defaultFile;
		bool _uploadingFile; //can be empty
		std::string _root;
		std::vector<std::string> _redirection;
		std::string _cgi;
	public:
		Site();
		~Site();

		void setName(std::string n);
		void setMethod(std::string s);
		void setListDirectory(bool val);
		void setDefaultFile(std::string s);
		void setUploadingFile(bool val);
		void setRoot(std::string s);
		void setRedirection(std::string s);
		void setCGI(std::string s);

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