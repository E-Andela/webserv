/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ParseHTTP.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: diwang <diwang@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2025/08/29 18:46:51 by diwang        #+#    #+#                 */
/*   Updated: 2025/09/07 17:23:43 by diwang        ########   odam.nl         */
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
		send_error_response(400, "Bad Request");
		return ;
	}
	buffer[bytes] = '\0';
	std::string http_request(buffer);
	size_t line_end = http_request.find("\r\n");
	if (line_end == std::string::npos)
	{
		send_error_response(400, "Bad Request");
		return ;
	}
	
	std::string line = http_request.substr(0, line_end);
	std::istringstream iss(line);
	std::string method1, path1, version1;
	if (!(iss >> method1 >> path1 >> version1))
	{
		send_error_response(400, "Bad Request");
		return ;
	}

	method = method1;  
	path = path1;
	version = version1;   
	
	if ((method != "GET" && method != "POST" && method != "DELETE") || path.empty() || path[0] != '/' ||
		 (version != "HTTP/1.1" && version != "HTTP/1.0"))
	{
		send_error_response(400, "Bad Request");
		return ;
	}
	

	if (method == "GET")
	{
		std::ifstream file("." + path, std::ios::binary);
		if (!file)
		{
			send_error_response(404, "Not Found");
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
		size_t header_end = http_request.find("\r\n\r\n");
		if (header_end == std::string::npos)
		{
			//throw an error// send 400 bad request
			return ;
		}
		std::string header_block = http_request.substr(line_end + 2, header_end - (line_end + 2));
		
		int content_length = 0;
		std::istringstream header_stream(header_block);
		std::string header_line;
		while (std::getline(header_stream, header_line))
		{
			if (!header_line.empty() && header_line.back() == '\r')
				header_line.pop_back();


			size_t colon_pos = header_line.find(':');
			if (colon_pos == std::string::npos)	continue;
			
			std::string key = header_line.substr(0, colon_pos);
			std::string value = header_line.substr(colon_pos + 1);

			key.erase(0, key.find_first_not_of(" \t"));
			key.erase(key.find_last_not_of(" \t") + 1);
			value.erase(0, value.find_first_not_of(" \t"));
			value.erase(value.find_last_not_of(" \t") + 1);

			std::transform(key.begin(), key.end(), key.begin(), ::tolower);
			if (key == "content-length")
			{
				try
				{
					content_length = std::stoi(value);
				}
				catch (const std::exception& e)
				{
					//throw an error// send 400 bad request
					return ;
				}
			}	
		}

		size_t content_start = header_end + 4;
		std::string body = http_request.substr(content_start);

		while ((int)body.size() < content_length)
		{
			char buffer[4096];
			ssize_t bytes = recv(client_socket, buffer, sizeof(buffer), 0);
			if (bytes <= 0) break;

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


void ParseHTTP::parse_http_request()
{
    constexpr size_t BUFFER_SIZE = 4096;
    bool keep_alive = false;

    do {
        char buffer[BUFFER_SIZE];
        ssize_t bytes = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes <= 0) {
            return;
        }
        buffer[bytes] = '\0';
        std::string http_request(buffer);

        size_t line_end = http_request.find("\r\n");
        if (line_end == std::string::npos) {
            send_error_response(400, "Bad Request");
            return;
        }

        std::string request_line = http_request.substr(0, line_end);
        std::istringstream iss(request_line);
        std::string method1, path1, version1;
        if (!(iss >> method1 >> path1 >> version1)) {
            send_error_response(400, "Bad Request");
            return;
        }

        method = method1;
        path = path1;
        version = version1;

        if ((method != "GET" && method != "POST" && method != "DELETE") ||
            path.empty() || path[0] != '/' ||
            (version != "HTTP/1.1" && version != "HTTP/1.0")) 
        {
            send_error_response(400, "Bad Request");
            return;
        }

        // --- Parse headers ---
        size_t header_end = http_request.find("\r\n\r\n");
        if (header_end == std::string::npos) {
            send_error_response(400, "Bad Request");
            return;
        }
        std::string header_block = http_request.substr(line_end + 2, header_end - (line_end + 2));

        std::map<std::string, std::string> headers;
        int content_length = 0;
        keep_alive = false;

        std::istringstream header_stream(header_block);
        std::string header_line;
        while (std::getline(header_stream, header_line)) {
            if (!header_line.empty() && header_line.back() == '\r')
                header_line.pop_back();

            size_t colon_pos = header_line.find(':');
            if (colon_pos == std::string::npos) continue;

            std::string key = header_line.substr(0, colon_pos);
            std::string value = header_line.substr(colon_pos + 1);

            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            headers[key] = value;

            if (key == "content-length") {
                try { content_length = std::stoi(value); } 
                catch (...) { send_error_response(400, "Bad Request"); return; }
            }
            if (key == "connection" && value == "keep-alive") {
                keep_alive = true;
            }
        }

        // --- Handle GET ---
        if (method == "GET") {
            std::ifstream file("." + path, std::ios::binary);
            if (!file) {
                send_error_response(404, "Not Found");
                return;
            }

            if (file.tellg() != 0) file.seekg(0); // Ensure we start from beginning
            char chunk[BUFFER_SIZE];
            std::string header = "HTTP/1.1 200 OK\r\n"
                                 "Content-Type: text/html\r\n"
                                 "Transfer-Encoding: chunked\r\n";
            if (keep_alive)
                header += "Connection: keep-alive\r\n";
            header += "\r\n";
            send(client_socket, header.c_str(), header.size(), 0);

            while (file) {
                file.read(chunk, BUFFER_SIZE);
                std::streamsize n = file.gcount();
                if (n <= 0) break;

                std::ostringstream chunk_size;
                chunk_size << std::hex << n << "\r\n";
                send(client_socket, chunk_size.str().c_str(), chunk_size.str().size(), 0);
                send(client_socket, chunk, n, 0);
                send(client_socket, "\r\n", 2, 0);
            }

            // end chunk
            send(client_socket, "0\r\n\r\n", 5, 0);
        }
        // --- Handle POST ---
        else if (method == "POST") {
            size_t content_start = header_end + 4;
            std::string body;
            if (http_request.size() > content_start)
                body = http_request.substr(content_start);

            while ((int)body.size() < content_length) {
                char buf[BUFFER_SIZE];
                ssize_t n = recv(client_socket, buf, BUFFER_SIZE, 0);
                if (n <= 0) break;
                body.append(buf, n);
            }

            std::string response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: " + std::to_string(body.size()) + "\r\n";
            if (keep_alive) response += "Connection: keep-alive\r\n";
            response += "\r\n" + body;
            send(client_socket, response.c_str(), response.size(), 0);
        }
        // --- Handle DELETE ---
        else if (method == "DELETE") {
            std::string file_path = "." + path;
            if (std::remove(file_path.c_str()) == 0) {
                std::string content = "<h1>Deleted " + path + "</h1>";
                std::string response =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: " + std::to_string(content.size()) + "\r\n";
                if (keep_alive) response += "Connection: keep-alive\r\n";
                response += "\r\n" + content;
                send(client_socket, response.c_str(), response.size(), 0);
            } else {
                send_error_response(404, "Not Found");
            }
        }

    } while (keep_alive);
}

std::string ParseHTTP::get_mime_type(const std::string& path) {
    size_t dot = path.rfind('.');
    if (dot == std::string::npos) return "text/plain";

    std::string ext = path.substr(dot + 1);
    if (ext == "html" || ext == "htm") return "text/html";
    if (ext == "css") return "text/css";
    if (ext == "js") return "application/javascript";
    if (ext == "json") return "application/json";
    if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
    if (ext == "png") return "image/png";
    if (ext == "gif") return "image/gif";
    if (ext == "svg") return "image/svg+xml";
    if (ext == "txt") return "text/plain";
    if (ext == "ico") return "image/x-icon";
    // Add more extensions as needed
    return "application/octet-stream"; // fallback binary
}


"Content-Type: text/html\r\n"


replace with - "Content-Type: " + get_mime_type(path) + "\r\n"

