#include "Location.hpp"


Location::Location():  _listDirectory(false), _uploadingFile(false)
{

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

void Location::setDefaultFile(std::string s)
{
	this->_defaultFile = s;
}

void Location::setUploadingFile(bool val)
{
	this->_uploadingFile = val;
}

void Location::setRoot(std::string s)
{
	this->_root = s;
}

void Location::setRedirection(std::string s)
{
	this->_redirection.push_back(s);
}

void Location::setCGI(std::string s)
{
	this->_cgi = s;
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
	std::cout << this->_defaultFile << std::endl;
}

void Location::printUploadingFile()
{
	std::cout << "uploading file :" << std::endl;
		if (this->_uploadingFile)
		std::cout << "true" << std::endl;
	else
		std::cout << "false" << std::endl;
}

void Location::printRoot()
{
	std::cout << "root :" << std::endl;
	std::cout << this->_root << std::endl;
}

void Location::printRedirection()
{
	std::cout << "redirection : " << std::endl;
	for (std::vector<std::string>::iterator it = this->_redirection.begin();
		it != this->_redirection.end();
		++it)
	{
		std::cout << *it << std::endl;
	}
}

void Location::printCGI()
{
	std::cout << "CGI :" << std::endl;
	std::cout << this->_cgi << std::endl;
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