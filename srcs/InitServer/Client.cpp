#include "Client.hpp"

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

	// provisoire ce que je met dedans
	_response = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 50\n\n<h1>bonjour ok voila</h1>";
}

Client::Client(const Client& other)
{
	_id = other._id;
	_request = other._request;
	_response = other._response;
	_readyToSend = other._readyToSend;
	_byteSend = other._byteSend;
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
	}
	return (*this);
}

Request& Client::getRequest()
{
	return _request;
}

Client::~Client()
{
	_readyToSend = false;
}

void Client::processRequest(const char *buffer, int size)
{
    setLastTime(time(NULL));

    bool success = _request.parse(buffer, size);

    if (success == false)
    {
        std::cout << "Parsing error : " << _request.getErrorCode() << std::endl;
        _request.setErrorCode(400);
        _readyToSend = true;
        return ;
    }
    if (_request.isComplete())
    {
        // DEBUG
        std::cout << "Requete complete recu !" << std::endl;
        std::cout << "Methode : " << _request.getMethod() << std::endl;
        // DEBUG

        std::string host = _request.getHeader("host");

        struct sockaddr_in sin;
        socklen_t len = sizeof(sin);
        int port = 80;

        if (getsockname(_id, (struct sockaddr *)&sin, &len) != -1)
            port = ntohs(sin.sin_port);
        _server = _webServ->findServer(_webServ->getServers(), host, port);
        if (_server == NULL) 
        {
            std::cout << "Erreur 500 : Aucun serveur trouvé pour ce port/host !" << std::endl;
            _request.setErrorCode(500);
            _readyToSend = true;
            return;
        }
        std::string uri = _request.getPath();
        _location = _webServ->findLocation(_server, uri);
        if (_location == NULL) 
        {
            std::cout << "Erreur 404 : Route non trouvée pour " << uri << std::endl;
            _request.setErrorCode(404);
            _readyToSend = true;
            return;
        }
        if (_server->getMaxSize() > 0 && _request.getBody().size() > (size_t)_server->getMaxSize())
        {
            std::cout << "Erreur 413 : Payload Too Large" << std::endl;
            _request.setErrorCode(413);
            _readyToSend = true;
            return;
        }

        std::vector<std::string> methods = _location->getMethods();
        bool methodAllowed = false;
        for (size_t i = 0; i < methods.size(); ++i) 
        {
            if (methods[i] == _request.getMethod()) 
            {
                methodAllowed = true;
                break;
            }
        }
        if (!methodAllowed)
        {
            std::cout << "Erreur 405 : Method Not Allowed" << std::endl;
            _request.setErrorCode(405);
            _readyToSend = true;
            return;
        }

        // SUCCÈS
        std::cout << "Routing SUCCES -> Srv: " << _server->getServerName()[0] << " | Loc: " << _location->getName() << std::endl;
        
        // appeler ici la fonction de yan pour la reponse ou dans la boucle
        
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

int Client::getID() const
{
	return _id;
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
