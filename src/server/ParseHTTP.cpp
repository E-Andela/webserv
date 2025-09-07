/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ParseHTTP.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: diwang <diwang@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2025/08/29 18:46:51 by diwang        #+#    #+#                 */
/*   Updated: 2025/09/07 14:51:25 by diwang        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ParseHTTP.hpp"

ParseHTTP::ParseHTTP()
{
	
}

ParseHTTP::~ParseHTTP()
{
	
}

void ParseHTTP::parse_http_request()
{
	char buffer[4096];
	ssize_t bytes;

	bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
	if (bytes <= 0)
	{
		//throw an error// send 400 bad request
		return ;
	}
	buffer[bytes] = '\0';
	std::string http_request(buffer);
	size_t line_end = http_request.find("\r\n");
	if (line_end == std::string::npos)
	{
		//throw error// send 400 bad request
		return ;
	}
	
	std::string line = http_request.substr(0, line_end);
	std::istringstream iss(line);
	std::string method1, path1, version1;
	if (!(iss >> method1 >> path1 >> version1))
	{
		//throw an error//send 400 bad request
		return ;
	}

	method = method1;  
	path = path1;
	version = version1;   
	
	if ((method != "GET" && method != "POST" && method != "DELETE") || path.empty() || path[0] != '/' ||
		 (version != "HTTP/1.1" && version != "HTTP/1.0"))
	{
		//throw an error;// send 400 bad request
		return ;
	}
	

	if (method == "GET")
	{
		std::ifstream file("." + path);
		if (!file)
		{
			//thrown an error;// send 404 not found
			return ;
		}
	
		std::stringstream get_content;
		get_content << file.rdbuf();
		std::string content = get_content.str();
		std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: " + std::to_string(content.size()) + "\r\n"
			"\r\n" +
			content;
		send(client_socket, response.c_str(), response.size(), 0);
	}
	else if (method == "POST")
	{
		   
	}
	else if (method == "DELETE")
	{
		std::string 
		  
	}

	
}

//parse with GET
// map / key
// what if it is GETA