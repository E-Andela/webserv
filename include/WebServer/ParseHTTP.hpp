/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ParseHTTP.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: diwang <diwang@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2025/08/29 18:49:16 by diwang        #+#    #+#                 */
/*   Updated: 2025/09/07 17:12:27 by diwang        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <string>
#include <cstring>
#include <cctype>
#include <cstdio>


#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

class ParseHTTP 
{
	public:
		ParseHTTP();
		~ParseHTTP();
		void parse_http_request();
		void send_error_response(int status_code, const std::string& message);
		
	private:
		std::map<std::string, std::string> btc_map;
		std::string method;
		std::string path;
		std::string version;
	
};

