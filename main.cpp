#include <iostream>
#include <string>

int main()
{
	while (1)
	{
		std::string input;
		std::cin >> input;

		if (input == "exit")
			break;
			
		std::cout << "input : " << input << std::endl;
	}

	return (0);
}