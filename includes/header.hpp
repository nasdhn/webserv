#ifndef HEADER_HPP
# define HEADER_HPP

# include <iostream>
# include <fstream>
# include <string>
# include <cstdlib>
# include <sstream>
# include <list>
# include <sstream>
# include <cctype>
# include <clocale>
# include <algorithm>
# include <map>
# include <vector>

# include "struct.hpp"
# include "colors.hpp"
# include "Server.hpp"
# include "WebServ.hpp"

//delete.cpp
std::string delWhiteSpace(std::string s);
std::string delSemiColon(std::string s);

//check.cpp
int haveSemiColon(std::string s);
int checkEmbrace(std::string s);
int checkBoolean(std::string s);
int checkMethod(std::string s);
int checkOnlySpace(std::string s);
int checkOnlyNumber(std::string s);
int isDigits(std::string str);
int checkHTTPCode(unsigned int code);
bool checkContent(const std::vector<std::string>& v, const std::string& texte);
bool checkDoubleListen(const std::vector<listenUrl>& v, const listenUrl& s);

#endif