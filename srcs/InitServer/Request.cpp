#include "Request.hpp"

/* ************************************************************************** */
/* Analyseur de requêtes HTTP 1.1. Transforme le flux d'octets bruts (TCP)    */
/* en un objet structuré et utilisable par le serveur.                        */
/* 																			  */
/* Fonctionnalités clés :                                                     */
/* - Machine à états : Lit ligne de départ -> En-têtes -> Corps (Body).       */
/* - Décodage Chunked : Gère automatiquement le 'Transfer-Encoding: chunked'. */
/* - Nettoyage : Décode les URI (enlève les %20) et sépare la Query String.   */
/* - Sécurité : Vérifie les erreurs (400 Bad Request, 505 Version HTTP).      */
/* - Protection : Bloque la requête si le body dépasse la limite configurée   */
/* dans le serveur (Erreur 413).                                              */
/* 																			  */
/* ************************************************************************** */


// A FAIRE : MAX SIZE DEPUIS LE PARSING A INCLURE

Request::Request()
{
	_method = "";
	_uri= "";
	_httpVersion = "";
	_body = "";
	_tempBuffer = "";
	_contentLength = 0;
	_maxBodySize = 2147483648;
	_state = REQ_START_LINE;
	_errorCode = 0;
	_isChunked = false;
	_chunkSize = 0;

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
	_chunkSize = other._chunkSize;
	_isChunked = other._isChunked;
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

// Dans Request.cpp
void Request::clear()
{
    _method = "";
    _uri = "";
    _path = "";
    _query = "";
    _httpVersion = "";
    _body = "";
    _headers.clear();
    _state = REQ_START_LINE;
    _errorCode = 0;
    _tempBuffer = ""; 
    _isChunked = false;
    _chunkSize = 0;
    _contentLength = 0;
    _maxBodySize = 2147483648;
}

void Request::setErrorCode(int code)
{
    _errorCode = code;
}

void Request::setMaxBodySize(size_t size)
{
    _maxBodySize = size;
}

std::string Request::getPath() const
{
	return _path;
}

std::string Request::getQuery() const
{
	return _query;
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

size_t Request::getMaxBodySize() const
{
	return(_maxBodySize);
}

bool Request::isComplete() const
{
    return (_state == REQ_COMPLETE);
}

int Request::getErrorCode() const
{
    return _errorCode;
}

bool Request::headerParsed() const
{
	return (_state > REQ_HEADERS);
}

size_t Request::getCurrentBodySize() const
{
	return _body.size();
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
			case REQ_CHUNKED_BODY:
				if (parseChunkBody() == false)
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
	parseUri();
	if (_httpVersion != "HTTP/1.1")
	{
		_errorCode = 505;
		_state = REQ_ERROR;
		return false;
	}
	// DEBUG
	std::cout << "Methode : " << _method << std::endl;
	std::cout << "URI : " << _uri << std::endl;
	std::cout << "Path : " << _path << std::endl;
	std::cout << "Query : " << _query << std::endl;
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
			if (_isChunked)
				_state = REQ_CHUNKED_BODY;
			else if (_contentLength > 0)
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

		if (key == "transfer-encoding" && value.find("chunked") != std::string::npos)
				_isChunked = true;
		else if (key == "content-length" && _isChunked == false)
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

static std::string urlDecode(const std::string &str)
{
    std::string decoded;
    char ch;
    int ii;

    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] == '%')
        {
            if (i + 2 < str.length())
            {
                std::istringstream hex(str.substr(i + 1, 2));
                if (hex >> std::hex >> ii)
                {
                    ch = static_cast<char>(ii);
                    decoded += ch;
                    i += 2;
                }
                else
                    decoded += str[i];
            }
            else
                decoded += str[i];
        }
        else if (str[i] == '+')
            decoded += ' ';
        else
            decoded += str[i]; 
    }
    return decoded;
}

void Request::parseUri()
{
	size_t pos = _uri.find('?');

	if (pos == std::string::npos)
	{	
		_path = urlDecode(_uri);
		_query = "";
	}
	else
	{
		_path = urlDecode(_uri.substr(0, pos));
		_query = _uri.substr(pos + 1);
	}
}

bool Request::parseChunkBody()
{
	std::string line = "";
	size_t needed = 0;

	while (true)
	{
        if (_body.size() > _maxBodySize) 
		{
        	_errorCode = 413;
        	_state = REQ_ERROR;
        	return false;
        }
		if (_chunkSize == 0)
		{
			size_t pos = _tempBuffer.find("\r\n");
			if (pos == std::string::npos)
				return true;
			else
			{
				line = _tempBuffer.substr(0, pos);
				std::stringstream ss;
				ss << std::hex << line;
				ss >> _chunkSize;
				_tempBuffer.erase(0, pos + 2);
				if (_chunkSize == 0)
				{	
					_state = REQ_COMPLETE;
					return true;
				}
			}
		}
		else
		{
			needed = _chunkSize + 2;
			if (_tempBuffer.size() < needed)
				return true;
			else
			{
				_body.append(_tempBuffer, 0, _chunkSize);
				_tempBuffer.erase(0, needed);
				_chunkSize = 0;
			}
		}

	}
}