/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ParseHTTP.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: diwang <diwang@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2025/08/29 18:46:51 by diwang        #+#    #+#                 */
/*   Updated: 2025/09/07 15:21:04 by diwang        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ParseHTTP.hpp"

ParseHTTP::ParseHTTP()
{
	
}

ParseHTTP::~ParseHTTP()
{
	
}

void ParseHTTP::send_error_response(int status_code, const std::string& message)
{
	std::string body = "<h1>" + std::to_string(status_code) + " " + message + "</h1>";
	std::string response =
		"HTTP/1.1 " + std::to_string(status_code) + " " + message + "\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(body.size()) + "\r\n"
		"\r\n" +
		body;
	send(client_socket, response.c_str(), response.size(), 0);
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
		size_t body_start = http_request.find("\r\n\r\n");
		if (body_start == std::string::npos)
		{
			//throw an error// send 400 bad request
			return ;
		}
		std::string body = http_request.substr(line_end + 2, body_start - (line_end + 2));
		
		int content_length = 0;
		std::istringstream header_stream(body);
		std::string header_line;
		while (std::getline(header_stream, header_line))
		{
			if (!header_line.empty() && header_line.back() == '\r')
				header_line.pop_back();


			size_t colon_pos = header_line.find(':');
			if (colon_pos != std::string::npos)	continue;
			
			std::string key = header_line.substr(0, colon_pos);
			std::string value = header_line.substr(colon_pos + 1);

			key.erase(0, key.find_first_not_of(" \t"));
			value.erase(0, value.find_first_not_of(" \t") + 1);
			value.erase(value.find_last_not_of(" \t") + 1);
		}

		size_t content_start = body_start + 4;
		std::string body = http_request.substr(content_start);

		while ((int)body.size() < content_length)
		{
			char buffer[4096];
			ssize_t bytes = recv(client_socket, buffer, sizeof(buffer), 0);
			if (n <= 0) break

			body.append(buffer, bytes);
		}
		std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain\r\n"
			"Content-Length: " + std::to_string(body.size()) + "\r\n"
			"\r\n" + 
			body;
		send(client_socket, response.c_str(), response.size(), 0);
	}
	else if (method == "DELETE")
	{
		std::string file_path = "." + path;
		if (std::remove(file_path.c_str()) == 0)
		{
			std::string body = "<h1>Deleted " + path + "</h1>";
			std::string response =
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: " + std::to_string(body.size()) + "\r\n"
				"\r\n" +
				body;
			send(client_socket, response.c_str(), response.size(), 0);
		}
		else
		{
			send_error_response(404, "Not Found");
		}

	}	

	
}

//parse with GET
// map / key
// what if it is GETA