#include <header.hpp>

int haveSemiColon(std::string s)
{
	if (s.find(";") < s.length())
		return (1);
	return 0;
}

int checkEmbrace(std::string s)
{
	for (size_t i = 0; i < s.length(); i++)
	{
		if (s[i] != '}' && !isspace(s[i]))
			return (1);
	}
	return 0;
}

int checkBoolean(std::string s)
{
	if (s == "true" || s == "false")
		return (0);
	return (1);
}

int checkMethod(std::string s)
{
	if (s == "GET" || s == "POST" || s == "DELETE")
		return (0);
	return (1);
}

int checkOnlySpace(std::string s)
{
	for(size_t i = 0; i < s.length(); i++)
	{
		if (!isspace(s[i]))
			return (0);
	}
	return (1);
}

int checkOnlyNumber(std::string s)
{
	for (size_t i = 0; i < s.length(); i++)
	{
		if (!isdigit(s[i]))
			return (0);
	}
	return (1);
}

int isDigits(std::string str)
{
	for (int i = 0; (size_t)i < str.length(); i++) {
		if (!(str[i] >= 48 && str[i] <= 57)) {
			return false;
		}
	}
	return true;
}

int checkHTTPCode(unsigned int code)
{
	unsigned int codesArray[] = {
		400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411,
		412, 413, 414, 415, 416, 417, 418, 421, 422, 423, 424, 425,
		426, 428, 429, 431, 451,
		500, 501, 502, 503, 504, 505, 506, 507, 508, 510, 511
	};
	
	for (int i = 0; i < (int)(sizeof(codesArray) / sizeof(codesArray[0])); i++)
	{
		if (code == codesArray[i])
			return (0);
	}
	return (1);
}

bool checkContent(const std::vector<std::string>& v, const std::string& s)
{
	return std::find(v.begin(), v.end(), s) != v.end();
}

bool checkDoubleListen(const std::vector<listenUrl>& v, const listenUrl& s)
{
	for (std::vector<listenUrl>::const_iterator it = v.begin();
		it != v.end();
		++it)
    {
        if (it->host == s.host && it->port == s.port)
            return true;
    }
    return false;
}