/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgoldens <cgoldens@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 15:34:26 by cgoldens          #+#    #+#             */
/*   Updated: 2025/12/11 11:40:50 by cgoldens         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_CPP
# define PARSING_CPP

# include <string>
# include <map>

class Parsing
{
	private:
		int _maxSize;
		std::map<int, std::string> errorPage;
		std::string _hostname; //vector
		std::string _listen; //vector
		//location
			//methods list/vector
			//listDirectory bool
			//defaultFile string
			//uploadingFile bool
			//root string
			//redirection map
			//CGI string

	public:

};

#endif