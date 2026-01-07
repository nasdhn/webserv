#include "Site.hpp"


Site::Site():  _listDirectory(false), _uploadingFile(false)
{

}

Site::~Site()
{

}


void Site::setName(std::string n)
{
	this->_name = n;
}

void Site::setMethod(std::string s)
{
	this->_methods.push_back(s);
}

void Site::setListDirectory(bool val)
{
	this->_listDirectory = val;
}

void Site::setDefaultFile(std::string s)
{
	this->_defaultFile = s;
}

void Site::setUploadingFile(bool val)
{
	this->_uploadingFile = val;
}

void Site::setRoot(std::string s)
{
	this->_root = s;
}

void Site::setRedirection(std::string s)
{
	this->_redirection.push_back(s);
}

void Site::setCGI(std::string s)
{
	this->_cgi = s;
}

std::vector<std::string> Site::getMethods()
{
	return (this->_methods);
}

void Site::printMethods()
{
	std::cout << "methods : " << std::endl;
	for (std::vector<std::string>::iterator it = this->_methods.begin();
		it != this->_methods.end();
		++it)
	{
		std::cout << *it << std::endl;
	}
}

void Site::printListDirectory()
{
	std::cout << "list directory :" << std::endl;
	if (this->_listDirectory)
		std::cout << "true" << std::endl;
	else
		std::cout << "false" << std::endl;
}

void Site::printDefaultFile()
{
	std::cout << "default file :" << std::endl;
	std::cout << this->_defaultFile << std::endl;
}

void Site::printUploadingFile()
{
	std::cout << "uploading file :" << std::endl;
		if (this->_uploadingFile)
		std::cout << "true" << std::endl;
	else
		std::cout << "false" << std::endl;
}

void Site::printRoot()
{
	std::cout << "root :" << std::endl;
	std::cout << this->_root << std::endl;
}

void Site::printRedirection()
{
	std::cout << "redirection : " << std::endl;
	for (std::vector<std::string>::iterator it = this->_redirection.begin();
		it != this->_redirection.end();
		++it)
	{
		std::cout << *it << std::endl;
	}
}

void Site::printCGI()
{
	std::cout << "CGI :" << std::endl;
	std::cout << this->_cgi << std::endl;
}

void Site::printData()
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