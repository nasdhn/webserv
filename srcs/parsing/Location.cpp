#include "Location.hpp"


Location::Location():  _listDirectory(false)
{
	this->_redirection.code = 0;
}

Location::~Location()
{

}


void Location::setName(std::string n)
{
	this->_name = n;
}

void Location::setMethod(std::string s)
{
	this->_methods.push_back(s);
}

void Location::setListDirectory(bool val)
{
	this->_listDirectory = val;
}

void Location::setIndex(std::string s)
{
	this->_index = s;
}

void Location::setUploadPath(std::string s)
{
	this->_uploadPath = s;
}

void Location::setRoot(std::string s)
{
	this->_root = s;
}

void Location::setRedirection(redir r)
{
	this->_redirection = r;
}

void Location::setCGI(cgi c)
{
	this->_cgi.push_back(c);
}

std::vector<std::string> Location::getMethods()
{
	return (this->_methods);
}

void Location::printMethods()
{
	std::cout << "methods : " << std::endl;
	for (std::vector<std::string>::iterator it = this->_methods.begin();
		it != this->_methods.end();
		++it)
	{
		std::cout << *it << std::endl;
	}
}

void Location::printListDirectory()
{
	std::cout << "list directory :" << std::endl;
	if (this->_listDirectory)
		std::cout << "true" << std::endl;
	else
		std::cout << "false" << std::endl;
}

void Location::printDefaultFile()
{
	std::cout << "default file :" << std::endl;
	std::cout << this->_index << std::endl;
}

void Location::printUploadingFile()
{
	std::cout << "uploading path :" << std::endl;
	std::cout << this->_uploadPath << std::endl;
}

void Location::printRoot()
{
	std::cout << "root :" << std::endl;
	std::cout << this->_root << std::endl;
}

void Location::printRedirection()
{
	std::cout << "redirection : " << std::endl;
	std::cout << this->_redirection.code << " " << this->_redirection.link << std::endl;
	
}

void Location::printCGI()
{
	std::cout << "CGI :" << std::endl;
	for (std::vector<cgi>::iterator it = this->_cgi.begin();
		it != this->_cgi.end();
		++it)
	{
		std::cout << it->ext << " " << it->path << std::endl;
	}
}

void Location::printData()
{
	std::cout << "============= " << this->_name << " =============" << std::endl;
	this->printMethods();
	this->printListDirectory();
	this->printDefaultFile();
	this->printUploadingFile();
	this->printRoot();
	this->printRedirection();
	this->printCGI();
}