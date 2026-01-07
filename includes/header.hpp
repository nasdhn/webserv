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

# include "colors.hpp"

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

#endif