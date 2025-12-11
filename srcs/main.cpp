#include "Server.hpp"

int main()
{
	try
	{
		Server s;

		s.setupServ();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	return 0;
}


