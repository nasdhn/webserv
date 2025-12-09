#include "Serveur.hpp"

int main()
{

	int fd;

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	// addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_zero[(sizeof(addr))];

	socklen_t client_len = sizeof(addr);

	while (1)
	{
		std::cout << "Attente de connexion.." << std::endl;
		fd = socket(AF_INET, SOCK_STREAM, 0);
		bind(fd, (struct sockaddr*)&addr, sizeof(addr));
		listen(fd, 1);
		int acpt = accept(fd, (struct sockaddr*)&addr, &client_len);

		write(acpt, "Test\n", 5);

		close(fd);
		close(acpt);
	}


	return (0);
}