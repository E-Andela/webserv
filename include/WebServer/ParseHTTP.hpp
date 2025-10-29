/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ParseHTTP.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: diwang <diwang@student.42.fr>                +#+                     */
/*                                                   +#+                      */
/*   Created: 2025/08/29 18:49:16 by diwang        #+#    #+#                 */
/*   Updated: 2025/10/27 18:34:40 by diwang        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include "Config/ServerConfig.hpp"
#include "unistd.h"
#include "sys/wait.h"

class Client;

class ParseHTTP 
{
	public:
		ParseHTTP();
		~ParseHTTP();

		void setClient(Client *client);
		void setConfig(const ServerConfig *config);
		std::string getResponse() const;
		void parse_http_request();

	private:
		Client *client;
		const ServerConfig *config;
		const RouteConfig *currentRoute;
		std::string response;
		std::string method;
		std::string path;
		std::string version;

		std::string getMimeType(const std::string &path);
		std::string urlConverter(const std::string &str); 
		std::string sanitizePath(const std::string &path);
		const RouteConfig *findRoute(const std::string &path);
		bool methodInConfig(const std::string &method, const RouteConfig *route);

		void handleGET();
		void handlePOST(const std::string &http_request, size_t line_end);
		void handleDELETE();
		void handleCGI();
		std::string executeCGI(const std::string& script_path, const std::string& query_string);
		std::vector<std::string> parseMultipartBody(const std::string &body, const std::string &boundary, const std::string &uploadPath);
		void error_response(int status_code, const std::string &message);
		

};

