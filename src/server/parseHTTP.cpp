/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parseHTTP.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: diwang <diwang@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2025/10/05 13:51:24 by diwang        #+#    #+#                 */
/*   Updated: 2025/10/08 18:41:28 by diwang        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer/ParseHTTP.hpp"
#include "WebServer/Client.hpp"
#include <sys/stat.h>
#include <algorithm>

ParseHTTP::ParseHTTP()
{
	
}

ParseHTTP::~ParseHTTP()
{
	
}

void ParseHTTP::setClient(Client* client)
{
	this->client = client;
}

void ParseHTTP::setConfig(const ServerConfig* config)
{
	this->config = config;
}

std::string ParseHTTP::getResponse() const
{
	return response;
}

std::string ParseHTTP::getMimeType(const std::string& path)
{
	size_t position = path.find_last_of('.');
	if (position == std::string::npos)
		return "application/octet-stream";
	
	std::string ext = path.substr(position + 1);
	for (size_t i = 0; i < ext.size(); ++i)
		ext[i] = std::tolower(ext[i]);
	
	if (ext == "html" || ext == "htm") 
		return "text/html";
	if (ext == "css") 
		return "text/css";
	if (ext == "js") 
		return "application/javascript";
	if (ext == "json") 
		return "application/json";
	if (ext == "xml") 
		return "application/xml";
	if (ext == "txt") 
		return "text/plain";
	if (ext == "jpg" || ext == "jpeg") 
		return "image/jpeg";
	if (ext == "png") 
		return "image/png";
	if (ext == "gif") 
		return "image/gif";
	if (ext == "ico") 
		return "image/x-icon";
	if (ext == "webp") 
		return "image/webp";
	if (ext == "pdf")
		return "application/pdf";
	if (ext == "zip") 
		return "application/zip";
	if (ext == "mp4") 
		return "video/mp4";
	if (ext == "webm") 
		return "video/webm";
	if (ext == "wav") 
		return "audio/wav";
	
	return "application/octet-stream";
}

std::string ParseHTTP::urlConverter(const std::string& str)
{
	std::string decoded;
	size_t i = 0;
	
	while (i < str.length())
	{
		if (str[i] == '%' && i + 2 < str.length())
		{
			std::string hex = str.substr(i + 1, 2);
			char ch = static_cast<char>(std::stoi(hex, nullptr, 16));
			decoded = decoded + ch;
			i = i + 3;
		}
		else if (str[i] == '+')
		{
			decoded = decoded + ' ';
			i++;
		}
		else
		{
			decoded = decoded + str[i];
			i++;
		}
	}
	
	return decoded;
}

// Sanitize path to prevent directory traversal ******
std::string ParseHTTP::sanitizePath(const std::string& path)
{
	std::string sanitized = urlConverter(path);
	
	size_t pos;
	while ((pos = sanitized.find("..")) != std::string::npos)
	{
		sanitized.erase(pos, 2);
	}
	
	while ((pos = sanitized.find("//")) != std::string::npos)
	{
		sanitized.erase(pos, 1);
	}
	
	if (sanitized.empty() || sanitized[0] != '/')
		sanitized = "/" + sanitized;
	
	return sanitized;
}


const RouteConfig* ParseHTTP::findRoute(const std::string& path)
{
	if (!config)
		return nullptr;
	
	std::cerr << "=== FINDING ROUTE ===" << std::endl;
	
	const RouteConfig* bestMatch = nullptr;
	size_t longestMatch = 0;
	
	for (const auto& route : config->routes)
	{
		// Check if path starts with route.path
		if (path.find(route.path) == 0)
		{
			// Make sure it's an exact match or followed by / or end of string
			size_t route_len = route.path.length();
			if (route_len > path.length())
				continue;
			// Exact match or route ends with / or path continues with /
			if (path.length() == route_len || route.path[route_len - 1] == '/' || path[route_len] == '/')
			{
				if (route_len > longestMatch)
				{
					bestMatch = &route;
					longestMatch = route_len;
					std::cerr << "  → Match found: '" << route.path << "'" << std::endl;
				}
			}
		}
	}
	if (!bestMatch)
		std::cerr << "  → NO MATCH FOUND!" << std::endl;
	
	std::cerr << "=== END FINDING ROUTE ===" << std::endl;
	
	return bestMatch;
}


bool ParseHTTP::methodInConfig(const std::string& method, const RouteConfig* route)
{
	if (!route)
		return false;
	for (const std::string &allowed : route->methods)
	{
		if (allowed == method)
			return true;
	}
	
	return false;
}

void ParseHTTP::parse_http_request()
{
	if (!config)
	{
		send_error_response(500, "Internal Server Error: No configuration");
		return;
	}
	std::string http_request = client->getRequest();

	size_t line_end = http_request.find("\r\n");
	if (line_end == std::string::npos)
	{
		send_error_response(400, "Bad Request");
		return;
	}
	
	std::string line = http_request.substr(0, line_end);
	std::istringstream iss(line);
	std::string method1, path1, version1;
	if (!(iss >> method1 >> path1 >> version1))
	{
		send_error_response(400, "Bad Request");
		return;
	}

	method = method1;  
	path = sanitizePath(path1);
	version = version1;   
	
	if (path.empty() || path[0] != '/' || (version != "HTTP/1.1"))
	{
		send_error_response(400, "Bad Request");
		return;
	}
	
	const RouteConfig* route = findRoute(path);
	
	if (!route)
	{
		std::cerr << "404: Route not found" << std::endl;
		send_error_response(404, "Not Found");
		return;
	}
	if (!methodInConfig(method, route))
	{
		send_error_response(405, "Method in Config File");
		return;
	}
	
	if (!route->redirectTo.empty())
	{
		response = 
			"HTTP/1.1 301 Moved Permanently\r\n"
			"Location: " + route->redirectTo + "\r\n"
			"Content-Length: 0\r\n"
			"\r\n";
		return;
	}
	
	currentRoute = route;
	
	if (method == "GET")
	{
		handleGET();
	}
	else if (method == "POST")
	{
		handlePOST(http_request, line_end);
	}
	else if (method == "DELETE")
	{
		handleDELETE();
	}
}

void ParseHTTP::handleGET()
{
	std::cerr << "=== HANDLE GET ===" << std::endl;
	std::cerr << "Original path: '" << path << "'" << std::endl;
	std::cerr << "Current route path: '" << currentRoute->path << "'" << std::endl;
	std::cerr << "Config root: '" << config->root << "'" << std::endl;
	std::cerr << "Config index: '" << config->index << "'" << std::endl;
	
	std::string file_path;
	
	if (path == "/" || path == currentRoute->path)
	{
		if (!config->index.empty())
		{
			path = "/" + config->index;
			std::cout << "TESTING HERE: " << path << std::endl;
		}
		else
			path = "/index.html";
		std::cerr << "Using index, new path: '" << path << "'" << std::endl;
	}
	
	if (!currentRoute->uploadPath.empty() && path.find(currentRoute->path) == 0)
	{
		std::string relative = path.substr(currentRoute->path.length()); // would like to combine these
		file_path = currentRoute->uploadPath + relative;
		std::cerr << "Serving from upload path: '" << file_path << "'" << std::endl;
	}
	else
	{
		file_path = config->root + path;
		std::cerr << "Serving from root: '" << file_path << "'" << std::endl;
	}
	
	std::cerr << "Attempting to open: '" << file_path << "'" << std::endl;
	
	std::ifstream file(file_path, std::ios::binary);
	
	if (!file)
	{
		std::cerr << "ERROR: File not found! === END HANDLE GET ===" << std::endl;
		send_error_response(404, "Not Found");
		return;
	}
	
	std::cerr << "File opened successfully!" << std::endl;
	
	std::stringstream get_content;
	get_content << file.rdbuf();
	std::string content = get_content.str();
	
	std::cerr << "Content size: " << content.size() << " bytes" << std::endl;
	
	std::string mime_type = getMimeType(file_path);
	
	response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: " + mime_type + "\r\n"
		"Content-Length: " + std::to_string(content.size()) + "\r\n"
		"\r\n" +
		content;
	
	std::cerr << "=== END HANDLE GET ===" << std::endl;
}

void ParseHTTP::handlePOST(const std::string& http_request, size_t line_end)
{
	std::cerr << "=== HANDLE POST ===" << std::endl;
	std::cerr << "Path: '" << path << "'" << std::endl;
	std::cerr << "Current route path: '" << currentRoute->path << "'" << std::endl;
	std::cerr << "Upload path: '" << currentRoute->uploadPath << "'" << std::endl;
	
	size_t header_end = http_request.find("\r\n\r\n");
	if (header_end == std::string::npos)
	{
		send_error_response(400, "Bad Request: malformed headers");
		return;
	}
	
	std::string header_block = http_request.substr(line_end + 2, header_end - (line_end + 2));

	std::string boundary;
	int content_length = 0;

	// Parse headers
	std::istringstream header_stream(header_block);
	std::string header_line;
	while (std::getline(header_stream, header_line))
	{
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

		if (key == "content-length")
			content_length = std::stoi(value);
		else if (key == "content-type" && value.find("multipart/form-data") != std::string::npos)
		{
			size_t bpos = value.find("boundary=");
			if (bpos != std::string::npos)
				boundary = "--" + value.substr(bpos + 9);
		}
	}

	if (boundary.empty())
	{
		send_error_response(400, "Bad Request: no boundary");
		return;
	}

	std::string body = http_request.substr(header_end + 4);

	// Check if route has upload path configured
	if (currentRoute->uploadPath.empty())
	{
		send_error_response(403, "Uploads not allowed for this route");
		return;
	}

	std::vector<std::string> uploaded_files = parseMultipartBody(body, boundary, currentRoute->uploadPath);
	
	if (uploaded_files.empty())
	{
		send_error_response(400, "No files found in request");
		return;
	}
	
	std::string responseBody = "<html><head><title>Upload Success</title></head><body>";
	responseBody += "<h1>Upload Successful</h1>";
	responseBody += "<p>Uploaded " + std::to_string(uploaded_files.size()) + " file(s):</p>";
	responseBody += "<ul>";
	for (const auto& filename : uploaded_files)
	{
		responseBody += "<li>" + filename + "</li>";
	}
	responseBody += "</ul>";
	responseBody += "<a href=\"/\">Back to home</a>";
	responseBody += "</body></html>";
	
	response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(responseBody.size()) + "\r\n"
		"\r\n" + responseBody;
}


std::vector<std::string> ParseHTTP::parseMultipartBody(const std::string& body, const std::string& boundary, const std::string& uploadPath)
{
	std::vector<std::string> uploaded_files;
	size_t pos = 0;
	
	while (true)
	{
		size_t boundary_pos = body.find(boundary, pos);
		if (boundary_pos == std::string::npos)
			break;
		
		pos = boundary_pos + boundary.length();
		
		if (pos + 2 <= body.length() && body.substr(pos, 2) == "--")
			break;
		
		if (pos + 2 <= body.length() && body.substr(pos, 2) == "\r\n")
			pos += 2;
		else if (pos < body.length() && body[pos] == '\n')
			pos += 1;
		
		size_t headers_end = body.find("\r\n\r\n", pos);
		if (headers_end == std::string::npos)
		{
			headers_end = body.find("\n\n", pos);
			if (headers_end == std::string::npos)
				continue;
			headers_end += 2;
		}
		else
			headers_end += 4;
		
		std::string part_headers = body.substr(pos, headers_end - pos);
		
		size_t filename_pos = part_headers.find("filename=\"");
		if (filename_pos == std::string::npos)
		{
			pos = headers_end;
			continue;
		}
		
		filename_pos += 10;
		size_t filename_end = part_headers.find("\"", filename_pos);
		if (filename_end == std::string::npos)
			continue;
		
		std::string filename = part_headers.substr(filename_pos, filename_end - filename_pos);
		
		size_t last_slash = filename.find_last_of("/\\");
		if (last_slash != std::string::npos)
			filename = filename.substr(last_slash + 1);
		
		if (filename.empty())
			continue;
		
		size_t content_start = headers_end;
		
		size_t next_boundary = body.find("\r\n" + boundary, content_start);
		if (next_boundary == std::string::npos)
		{
			next_boundary = body.find("\n" + boundary, content_start);
			if (next_boundary == std::string::npos)
				continue;
		}
		
		size_t content_end = next_boundary;
		std::string file_content = body.substr(content_start, content_end - content_start);
		
		// Ensure upload directory exists
		struct stat st;
		if (stat(uploadPath.c_str(), &st) != 0)
		{
			mkdir(uploadPath.c_str(), 0755);
		}
		
		std::string full_path = uploadPath + "/" + filename;
		std::ofstream out_file(full_path, std::ios::binary);
		if (out_file)
		{
			out_file.write(file_content.c_str(), file_content.size());
			out_file.close();
			uploaded_files.push_back(filename);
		}
		
		pos = content_end;
	}
	
	return uploaded_files;
}

void ParseHTTP::handleDELETE()
{
	// Check if route allows uploads (and thus deletions)
	if (currentRoute->uploadPath.empty())
	{
		send_error_response(403, "Forbidden");
		return;
	}
	
	// Build file path
	std::string relative = path.substr(currentRoute->path.length());
	std::string file_path = currentRoute->uploadPath + relative;
	
	if (std::remove(file_path.c_str()) == 0)
	{
		std::string body = "<html><body><h1>Deleted " + path + "</h1><a href=\"/\">Back to home</a></body></html>";
		response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: " + std::to_string(body.size()) + "\r\n"
			"\r\n" +
			body;
	}
	else
	{
		send_error_response(404, "Not Found");
	}
}

void ParseHTTP::send_error_response(int status_code, const std::string& message)
{

	std::cerr << "=== SENDING ERROR RESPONSE ===" << std::endl;
    std::cerr << "Status: " << status_code << " " << message << std::endl;
	
	std::string body;
	
	// Check for custom error page
	if (config && config->errorPages.count(status_code) > 0)
	{
		std::string error_page_path = config->root + "/" + config->errorPages.at(status_code);
		std::ifstream error_file(error_page_path, std::ios::binary);
		if (error_file)
		{
			std::stringstream ss;
			ss << error_file.rdbuf();
			body = ss.str();
		}
	}
	
	// Fallback to default error page
	if (body.empty())
	{
		body = "<html><body><h1>" + std::to_string(status_code) + " " + message + "</h1></body></html>";
	}
	
	response =
		"HTTP/1.1 " + std::to_string(status_code) + " " + message + "\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(body.size()) + "\r\n"
		"\r\n" +
		body;
}





