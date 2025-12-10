/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgoldens <cgoldens@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 15:34:29 by cgoldens          #+#    #+#             */
/*   Updated: 2025/12/10 15:41:53 by cgoldens         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <list>
#include <algorithm>

int main(int ac, char **av)
{
	std::ifstream file;
	std::string line;
	std::string delimiter = " ";
	std::string token;

	std::cout << ac << " " << av[ac - 1] << std::endl;
	file.open(av[ac - 1]);
	while (std::getline(file, line))
	{
		std::cout << line << std::endl;
	}
}
