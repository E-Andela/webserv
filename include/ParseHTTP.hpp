/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ParseHTTP.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: diwang <diwang@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2025/08/29 18:49:16 by diwang        #+#    #+#                 */
/*   Updated: 2025/09/07 14:34:00 by diwang        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <ifstream>
#include <sstream>
#include <map>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

class ParseHTTP 
{
	public:
		ParseHTTP();
		~ParseHTTP();
		void parse_http_request();
		
	private:
		std::map<std::string, std::string> btc_map;
		std::string method;
		std::string path;
		std::string version;
	
};

