#include "Location.hpp"


Location::Location()
{
	this->_redirection.code = 0;
	this->_autoIndex = 2;
	this->_uploadPath = "";
	this->_root = "";
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
	this->_autoIndex = val;
}

void Location::setIndex(std::string s)
{
	this->_index.push_back(s);
}

void Location::setUploadPath(std::string s)
{
	this->_uploadPath = s;
}

void Location::setRedirection(redir r)
{
	this->_redirection = r;
}

void Location::setCGI(cgi c)
{
	this->_cgi.push_back(c);
}

std::vector<std::string> Location::getMethods() const
{
	return (this->_methods);
}

std::string Location::getName() const
{
	return (this->_name);
}

std::string Location::getRoot()
{
	return (this->_root);
}

std::vector<cgi> Location::getCgi()
{
	return (this->_cgi);
}

int Location::getAutoIndex()
{
	return (this->_autoIndex);
}

std::vector<std::string> Location::getIndex()
{
	return _index;
}

std::string Location::getUploadPath()
{
	return (this->_uploadPath);
}

redir Location::getRedir()
{
	return (this->_redirection);
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
	if (this->_autoIndex)
		std::cout << "true" << std::endl;
	else
		std::cout << "false" << std::endl;
}

void Location::printDefaultFile()
{
	std::cout << "default file :" << std::endl;
		for (std::vector<std::string>::iterator it = this->_index.begin();
		it != this->_index.end();
		++it)
	{
		std::cout << *it << std::endl;
	}
}

void Location::printUploadingFile()
{
	std::cout << "uploading path :" << std::endl;
	std::cout << this->_uploadPath << std::endl;
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
	this->printRedirection();
	this->printCGI();
}