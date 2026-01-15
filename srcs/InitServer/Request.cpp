#include "Request.hpp"

Request::Request()
{
	_method = "";
	_uri= "";
	_httpVersion = "";
	_body = "";
	_tempBuffer = "";
	_contentLength = 0;
	_maxBodySize = 1048576; // mettre le truc de cyril
	_state = REQ_START_LINE;
	_errorCode = 0;

}

Request::Request(const Request& other)
{
	_method = other._method;
	_uri = other._uri;
	_httpVersion = other._httpVersion;
	_body = other._body;
	_tempBuffer = other._tempBuffer;
	_contentLength = other._contentLength;
	_maxBodySize = other._maxBodySize;
	_state = other._state;
	_errorCode = other._errorCode;
	_headers= other._headers;
}

Request& Request::operator=(const Request& other)
{
	if (this != &other)
	{
		_method = other._method;
		_uri = other._uri;
		_httpVersion = other._httpVersion;
		_body = other._body;
		_tempBuffer = other._tempBuffer;
		_contentLength = other._contentLength;
		_maxBodySize = other._maxBodySize;
		_state = other._state;
		_errorCode = other._errorCode;
		_headers= other._headers;
	}
	return (*this);
}

Request::~Request()
{

}

std::string Request::getMethod() const
{
	return _method;
}

std::string Request::getURI() const
{
	return _uri;
}

std::string Request::getVersion() const
{
	return _httpVersion;
}

std::string Request::getBody() const
{
	return _body;
}

bool Request::isComplete() const
{
    return (_state == REQ_COMPLETE);
}

int Request::getErrorCode() const
{
    return _errorCode;
}

std::string Request::getHeader(const std::string& key) const
{
	std::map<std::string, std::string>::const_iterator it;

    it = _headers.find(key);

    if (it != _headers.end())
        return it->second;
    return "";
}


bool Request::parse(const char* data, size_t size)
{
	if (size > 0 && data != NULL)
		_tempBuffer.append(data, size);

	bool stateChanged = true;

	while (stateChanged && _state != REQ_COMPLETE && _state != REQ_ERROR)
    {
        stateChanged = false;
        RequestState previousState = _state;
        switch (_state)
        {
            case REQ_START_LINE:
                if (parseStartLine() == false)
                    return false;
                break;
            case REQ_HEADERS:
                if (parseHeaders() == false)
                    return false;
                break;
            case REQ_BODY:
                if (parseBody() == false)
                    return false;
                break;
            case REQ_COMPLETE:
            case REQ_ERROR:
                break;
        }
        if (_state != previousState)
            stateChanged = true;
	}
	if (_state == REQ_ERROR)
    	return false;
	else
    	return true;
}

bool Request::parseStartLine()
{
	size_t pos = _tempBuffer.find("\r\n");

	if (pos == std::string::npos)
		return true;

	std::string line = _tempBuffer.substr(0, pos);
	_tempBuffer.erase(0, pos + 2);

	std::stringstream ss(line);
	ss >> _method >> _uri >> _httpVersion;

	if (_method.empty() || _uri.empty() || _httpVersion.empty())
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
	std::cout << "Methode : " << _method << std::endl;
	std::cout << "Path : " << _uri << std::endl;
	std::cout << "htpp Version : " << _httpVersion << std::endl;
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

		for (size_t i = 0; i < key.length(); i++)
			key[i] = std::tolower(key[i]);
		
		_headers[key] = value;

		if (key == "content-length")
		{
			std::istringstream iss(value);
			iss >> _contentLength; 
		}

	}
	return true;
}


bool Request::parseBody()
{
	if (_contentLength > _maxBodySize)
	{
		_errorCode = 413;
		_state = REQ_ERROR;
		return false;
	}

	size_t needed = _contentLength - _body.size();

	if (_tempBuffer.length() >= needed)
	{
		_body.append(_tempBuffer, 0, needed);
		_tempBuffer.erase(0, needed);
		_state = REQ_COMPLETE;
	}
	else
	{
		_body.append(_tempBuffer);
		_tempBuffer.clear();
	}

	return true;
}

void Request::parseUri()
{
	size_t pos = _uri.find('?');

	if (pos == std::string::npos)
	{	
		_path = _uri;
		_query = "";
	}
	else
	{
		_path = _uri.substr(0, pos);
		_query = _uri.substr(pos + 1);
	}
}