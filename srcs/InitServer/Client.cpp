#include "Client.hpp"
#include "../../includes/Response.hpp"

/* ************************************************************************** */
/* Représente un client connecté. C'est le chef d'orchestre qui gère le       */
/* cycle de vie d'une connexion, de la requête à la réponse.                  */
/* 																			  */
/* Responsabilités clés :                                                     */
/* 1. Réseau : Gère l'ID du socket et l'état de la connexion.                 */
/* 2. Parsing : Déclenche la classe 'Request' pour lire les données brutes.   */
/* 3. Routing (Le Cerveau) : Exécute 'processRequest' pour décider :          */
/* - Quel Serveur répond (match par Host/Port).                            	  */
/* - Quelle Location utiliser (match par préfixe le plus long).               */
/* - Vérifier la sécurité (Méthodes autorisées, Taille max du body).          */
/* 4. Passage de relais : Stocke les pointeurs _server et _location pour      */
/* que le CGI ou le générateur de réponse puisse travailler ensuite.          */
/* 																			  */
/* ************************************************************************** */

Client::Client(int fd, WebServ* webServ) : _webServ(webServ)
{
	_id = fd;
	_readyToSend = false;
	_byteSend = 0;
	_lastTime = time(NULL);
	_headersSent = false;
    _headerBuffer = "";
    _fileFD = -1; 
    _responseStr = "";
    _server = NULL;
    _location = NULL;
    _routingDone = false;
}

Client::Client(const Client& other)
{
	*this = other;
}

Client& Client::operator=(const Client& other)
{
	if (this != &other)
	{
		_id = other._id;
		_request = other._request;
		_response = other._response;
		_readyToSend = other._readyToSend;
		_byteSend = other._byteSend;

        _headersSent = other._headersSent;
        _headerBuffer = other._headerBuffer;
        _server = other._server;
        _location = other._location;
        _lastTime = other._lastTime;
        _fileFD = other._fileFD; 
        _responseStr = other._responseStr;
        _routingDone = other._routingDone;
	}
	return (*this);
}

Request& Client::getRequest()
{
	return _request;
}

bool& Client::getRoutingDone()
{
    return _routingDone;
}

void Client::reset()
{
    _routingDone = false;
    _headersSent = false;
    _server = NULL;
    _location = NULL;
    _request.clear();
}

Client::~Client()
{
    closeFile();
	_readyToSend = false;
}

void Client::closeFile() 
{ 
    if (_fileFD != -1) 
	{
		close(_fileFD);
		_fileFD = -1; 
	}
}

void Client::setFileFD(int n)
{
    _fileFD = n;
}

void Client::processRequest(const char *buffer, int size)
{
    setLastTime(time(NULL));

    bool success = _request.parse(buffer, size);

    if (success == false)
    {
        std::cout << "Parsing error : " << _request.getErrorCode() << std::endl;
        _response.setStatus(400);
        _response.setBody("<html><body><h1>400 Bad Request</h1></body></html>");
        _headerBuffer = _response.get_header() + "\r\n";
        _headersSent = false;
        _readyToSend = true;
        return ;
    }
    if (_routingDone == false && _request.getHeader("host").empty() == false)
    {
        std::string host = _request.getHeader("host");
        struct sockaddr_in sin;
        socklen_t len = sizeof(sin);
        int port = 80;

        if (getsockname(_id, (struct sockaddr *)&sin, &len) != -1)
            port = ntohs(sin.sin_port);
        
        _server = _webServ->findServer(_webServ->getServers(), host, port);
        
        if (_server == NULL) 
        {
            _response.setStatus(500);
            _response.setBody("<html><body><h1>500 Internal Server Error</h1></body></html>");
            _headerBuffer = _response.get_header() + "\r\n";
            _headersSent = false;
            _readyToSend = true;
            return;
        }

        std::string uri = _request.getPath();
        _location = _webServ->findLocation(_server, uri);

        size_t limit = _server->getMaxSize(); 
        if (_location)
             limit = _server->getMaxSize();

        _request.setMaxBodySize(limit);

        std::string cl = _request.getHeader("content-length");
        if (cl.empty() == false)
        {
            size_t len = std::atoll(cl.c_str());
            if (len > limit)
            {
                std::cout << "Erreur 413 : Content-Length (" << len << ") > Limit (" << limit << ")" << std::endl;
                _request.setErrorCode(413);
            }
        }
        
        _routingDone = true;
    }

    if (_routingDone == true && _request.getBody().size() > _request.getMaxBodySize())
    {
        _request.setErrorCode(413);
    }
    if (_request.getErrorCode() == 413)
    {
        _response.setStatus(413);
        _response.setBody("<html><body><h1>413 Request Entity Too Large</h1></body></html>");
        _response.setHeader("Connection", "close");
        _headerBuffer = _response.get_header() + "\r\n";
        _headersSent = false;
        _readyToSend = true;
        return;
    }
    if (_request.isComplete())
    {
		std::cout << "Requete complete recu !" << std::endl;
        std::cout << "Methode : " << _request.getMethod() << std::endl;
        std::cout << "Routing SUCCES -> Srv: " << _server->getServerName()[0] << std::endl;
        
        _response = Response(_request, _server, (Location*)_location);
        _headerBuffer = _response.get_header() + "\r\n";
        _headersSent = false;
        _readyToSend = true;

        _responseStr = _response.get_header();

        if (_response.is_fd())
        {
            _fileFD = _response.get_body_fd();
        }
        else
        {
            _responseStr += _response.get_body_string();
            _fileFD = -1;
        }
        _readyToSend = true;
    }
    else
    {
        std::cout << "Requete en cours de chargement..." << std::endl;
    }
}

void Client::setLastTime(time_t time)
{
	_lastTime = time;
}

int& Client::getFileFD()
{
    return _fileFD;
}

int Client::getID() const
{
	return _id;
}


std::string& Client::getResponse()
{
	return _responseStr;
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
