#include "WebServ.hpp"

int main()
{
	try
	{
		WebServ server;

		server.setupServ();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	return 0;
}