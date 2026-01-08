#include "Client.hpp"

Client::Client(int fd)
{
	_id = fd;
	_readyToSend = false;
	_byteSend = 0;
	_lastTime = time(NULL);

	// provisoire ce que je met dedans
	_contentSize_int = 0;
	_response = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 50\n\n<h1>bonjour ok voila</h1>";
}

Client::Client(const Client& other)
{
	_id = other._id;
	_request = other._request;
	_header = other._header;
	_response = other._response;
	_readyToSend = other._readyToSend;
	_byteSend = other._byteSend;


	// provisoire
	_contentLenght_str = other._contentLenght_str;
	_contentSize_int = other._contentSize_int;
}

Client& Client::operator=(const Client& other)
{
	if (this != &other)
	{
		_id = other._id;
		_request = other._request;
		_header = other._header;
		_response = other._response;
		_readyToSend = other._readyToSend;
		_byteSend = other._byteSend;


		// provisoire
		_contentLenght_str = other._contentLenght_str;
		_contentSize_int = other._contentSize_int;
	}
	return (*this);
}

Client::~Client()
{
	_readyToSend = false;
}

void Client::processRequest(const char *buffer, int size)
{
    setLastTime(time(NULL));
    _request.append(buffer, size);

	// DEBUG
	// std::cout << "Message recu : " << buffer << std::endl;
	// std::cout << "Message de id : " << client->getID() << " dans _client : " << client->getRequest() << std::endl;
	// std::cout << "Octet recu : " << ret << " | Total octet : " << client->getRequest().size() << std::endl;
	// DEBUG

	// le parsing ne gere pas les gros fichiers c est seulement temporaires, pas oublier de faire les modifs qund je pourrais
    if (_header.empty())
    {
        unsigned long pos = _request.find("\r\n\r\n");
        if (pos != std::string::npos)
        {
            _header = _request.substr(0, pos + 4);
            _request.erase(0, pos + 4); 

			// Faire une class request pour les request
			// code provisoire et variable a modifier une fois que la class sera faite
			// content_lenght et request
            std::string CL = "Content-Length";
            unsigned long CL_pos = _header.find(CL);
            if (CL_pos != std::string::npos)
            {
                unsigned long endLine = _header.find('\n', CL_pos);
                _contentLenght_str = _header.substr(CL_pos + 16, endLine - (CL_pos + 16));
                
                std::istringstream iss(_contentLenght_str);
                iss >> _contentSize_int;
                std::cout << "Taille attendue : " << _contentSize_int << std::endl;
            }
            else
            {
                // Pas de content-length  c'est fini tout de suite
                _readyToSend = true;
                std::cout << "Pas de CL, requête complète." << std::endl;
            }
        }
    }
    if (!_header.empty() && !_readyToSend)
    {
        if (_contentSize_int > 0)
        {
			// avec content lenght check le buffer si il a tout lu ou si il en reste c est ce qui va etre le body
            if ((size_t)_request.size() >= (size_t)_contentSize_int)
            {
                _readyToSend = true;
                std::cout << "Body entièrement lu" << std::endl;
            }
            else
            {
                std::cout << "Body pas encor tout lu... (" << _request.size() << "/" << _contentSize_int << ")" << std::endl;
            }
        }
    }
}

void Client::setLastTime(time_t time)
{
	_lastTime = time;
}

int Client::getID() const
{
	return _id;
}

std::string& Client::getRequest()
{
	return _request;
}

std::string& Client::getHeader()
{
	return _header;
}

std::string& Client::getResponse()
{
	return _response;
}

bool& Client::getReadyToSend()
{
	return _readyToSend;
}

unsigned long& Client::getByteSend()
{
	return _byteSend;
}

time_t& Client::getLastTime()
{
	return _lastTime;
}

// fonction provisoire a remplacer ou a suppr par celle de class Request

std::string& Client::getContentSizeString()
{
	return _contentLenght_str;
}

int& Client::getContentSizeInt()
{
	return _contentSize_int;
}