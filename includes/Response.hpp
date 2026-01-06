#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>

class Response {
    public:
        Response();
        ~Response();
        Response(const Response &other);
        Response &operator=(const Response &other);
        void setStatus(int code);
        void setHeader(const std::string& key, const std::string& value);
        void setBody(const std::string& body);
		//pour nasim
        std::string toString() const;

    private:
        int _status;
        std::string _body;
        std::map<std::string, std::string> _headers;
        std::string _getStatusMessage(int code) const;
};

#endif