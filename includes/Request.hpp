#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <iostream>
#include <cstdlib>
#include <sstream>


enum RequestState {
    REQ_START_LINE,
    REQ_HEADERS,
    REQ_BODY,
    REQ_CHUNKED_BODY,
    REQ_COMPLETE,
    REQ_ERROR
};

class Request {

    public:
        Request();
        Request(const Request& other);
        Request& operator=(const Request& other);
        ~Request();

        bool parse(const char* data, size_t size);

        bool isComplete() const;
        int  getErrorCode() const;

        std::string getMethod() const;
        std::string getVersion() const;
        std::string getBody() const;
        std::string getHeader(const std::string& key) const;
        std::string getURI() const;
        std::string getPath() const;
        std::string getQuery() const;

        void setErrorCode(int code);

        bool headerParsed() const;
        size_t getCurrentBodySize() const;
        
        void clear();


        void setMaxBodySize(size_t size);

    private:
        std::string _method;
        std::string _httpVersion;
        std::map<std::string, std::string> _headers;
        std::string _body;

        std::string _uri;
        std::string _path;
        std::string _query;


        std::string _tempBuffer; 
        size_t      _contentLength;
        size_t      _maxBodySize;
        RequestState _state;
        int          _errorCode;

        bool _isChunked;
        size_t _chunkSize;

        bool parseStartLine();
        bool parseHeaders();
        bool parseBody();
        bool parseChunkBody();
        void parseUri();
        


        // ajouter plus tard le pars chunked jspquoi
        
};

#endif