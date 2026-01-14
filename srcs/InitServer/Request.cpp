#include "Request.hpp"

Request::Request()
{

}

Request::Request(const Request& other)
{

}

Request& Request::operator=(const Request& other)
{

}

Request::~Request()
{

}



bool Request::parse(const char* data, size_t size)
{

}

bool Request::parseStartLine()
{
	size_t pos = _tempBuffer.find("\r\n");

	if (pos == std::string::npos)
		return true;

	std::string line = _tempBuffer.substr(0, pos);
	_tempBuffer.erase(0, pos + 2);


	std::stringstream ss(line);
	ss >> _method >> _path >> _httpVersion;

	if (_method.empty() || _path.empty() || _httpVersion.empty())
	{
		_errorCode = 400;
		_state = REQ_ERROR;
		return false;
	}

	if (_httpVersion != "HTTP/1.1")
	{
		_errorCode = 505;
		_state = REQ_ERROR;
		return false;
	}


	// DEBUG
	std::cout << "Methode : " <<_method << std::endl;
	std::cout << "Path : " <<_path << std::endl;
	std::cout << "htpp Version : " <<_httpVersion << std::endl;
	// DEBUG

	_state = REQ_HEADERS;
	return true;

}



bool Request::parseHeaders()
{
	std::string line;

	while (true)
	{
		size_t pos = _tempBuffer.find("\r\n");

		if (pos == std::string::npos)
			return true;

		if (pos == 0)
		{	
			_tempBuffer.erase(0, 2);
			if (_contentLength > 0)
				_state = REQ_BODY;
			else
				_state = REQ_COMPLETE;
			return true;
		}
		else
		{
			line = _tempBuffer.substr(0, pos);
			_tempBuffer.erase(0, pos + 2);
		}

		size_t posP = line.find(':');
		if (posP == std::string::npos)
		{
			_errorCode = 400;
			_state = REQ_ERROR;
			return false;
		}

		std::string key = line.substr(0, posP);
        std::string value = line.substr(posP + 1);

		if (!value.empty() && value[0] == ' ')
            value.erase(0, 1);
		_headers[key] = value;

		if (key == "Content-Length")
		{
			std::istringstream iss(value);
			iss >> _contentLength; 
		}

	}
	return true;
}


bool parseBody();