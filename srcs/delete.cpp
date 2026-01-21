#include <header.hpp>

std::string delWhiteSpace(std::string s)
{
	int i = 0;
	while (i < (int)s.length() && std::isspace(s[i]))
		i++;
	s.erase(0, i);
	return s;
}

std::string delSemiColon(std::string s)
{
	if (s.find(";") <= s.length())
	{
		int pos = s.find(";");
		s.erase(pos, pos + 1);
	}
	return (s);
}