// void ParseHTTP::parse_http_request()
// {
// 	if (!config)
// 	{
// 		error_response(500, "Internal Server Error: no config loaded");
// 		return ;
// 	}
// 	std::string request = client->getRequest();
	
// 	size_t line_end = request.find("\r\n");
// 	if (line_end == std::string::npos)
// 	{
// 		error_response(400, "bad request");
// 		return ;
// 	}
	
// 	std::string line = request.substr(0, line_end);
// 	std::istringstream iss(line);
// 	std::string method1, path1, version1;
// 	if (!(iss >> method1 >> path1 >> version1))
// 	{
// 		error_response(400, "bad request");
// 		return ;
// 	}
// 	method = method1;
// 	path = sanitizePath(path1);
// 	version = version1;

// 	if (path.empty() || path[0] != '/' || version != "HTTP/1.1")
// 	{
// 		error_response(400, "bad request");
// 		return ;
// 	}
// 	const RouteConfig *route = findRoute(path);
// 	if (!route)
// 	{
// 		error_response(404, "not found");
// 		return ;
// 	}
// 	if (!methodInConfig(method, route))
// 	{
// 		error_response(405, "no match in config file");
// 		return ;
// 	}
// 	if (!route->redirectTo.empty())
// 	{
// 		response =
// 			"HTTP/1.1 301 Moved permanently\r\n"
// 			"Location: " + route->redirectTo + "\r\n"
// 			"Content-Length: 0\r\n"
// 			"\r\n";
// 		return ;
// 	}
	
// 	currentRoute = route;

// 	if (!currentRoute->cgiPath.empty())
// 	{
// 		handleCGI();
// 		return;
// 	}
	
// 	if (method == "GET")
// 		handleGET();
// 	else if (method == "POST")
// 		handlePOST(request, line_end);
// 	else if (method == "DELETE")
// 		handleDELETE();
// 	else 
// 		{
// 			error_response(501, "method not implemented");
// 			return ;
// 		}
	
// }


// void ParseHTTP::handleGET()
// {

// 	std::cerr << "=== HANDLE GET ===" << std::endl;
// 	std::cerr << "Original path: '" << path << "'" << std::endl;
// 	std::cerr << "Current route path: '" << currentRoute->path << "'" << std::endl;
// 	std::cerr << "Config root: '" << config->root << "'" << std::endl;
// 	std::cerr << "Config index: '" << config->index << "'" << std::endl;
	
// 	std::string file_path;
	
// 	if (path == "/" || path == currentRoute->path)
// 	{
// 		if (!config->index.empty())
// 		{
// 			path = "/" + config->index;
// 		}	
// 		else
// 			path = "/index.html";
// 	}
// 	if (!currentRoute->uploadPath.empty() && path.find(currentRoute->path) == 0)
// 	{
// 		std::string relative = path.substr(currentRoute->path.length());
// 		file_path = currentRoute->uploadPath + relative;
// 	}
// 	else	
// 		file_path = config->root + path;
	
// 	std::ifstream file(file_path, std::ios::binary);
// 	if (!file)
// 	{
// 		struct stat st;
// 		if (stat(file_path.c_str(), &st) == 0)
// 		{
// 			error_response(403, "Forbidden");
// 		}
// 		else
// 		{
// 			error_response(404, "Not Found");
// 		}
// 		return ;
// 	}

// 	std::stringstream get_content;
// 	get_content << file.rdbuf();
// 	std::string content = get_content.str();

// 	std::string mime_type = getMimeType(file_path);

// 	response = 
// 		"HTTP/1.1 200 OK\r\n"
// 		"Content-Type: " + mime_type + "\r\n"
// 		"Content-Length: " + std::to_string(content.size()) + "\r\n"
// 		"\r\n" +
// 		content;
		
// }


// void ParseHTTP::handlePOST(const std::string& http_request, size_t line_end)
// {
// 	std::cerr << "=== HANDLE POST ===" << std::endl;
// 	std::cerr << "Path: '" << path << "'" << std::endl;
// 	std::cerr << "Current route path: '" << currentRoute->path << "'" << std::endl;
// 	std::cerr << "Upload path: '" << currentRoute->uploadPath << "'" << std::endl;
	
// 	size_t header_end = http_request.find("\r\n\r\n");
// 	if (header_end == std::string::npos)
// 	{
// 		error_response(400, "bad request");
// 		return;
// 	}
	
// 	std::string header_block = http_request.substr(line_end + 2, header_end - (line_end + 2));

// 	std::string boundary;
// 	int content_length = 0;

// 	// Parse headers
// 	std::istringstream header_stream(header_block);
// 	std::string header_line;
// 	while (std::getline(header_stream, header_line))
// 	{
// 		if (!header_line.empty() && header_line.back() == '\r')
// 			header_line.pop_back();

// 		size_t colon_pos = header_line.find(':');
// 		if (colon_pos == std::string::npos) continue;

// 		std::string key = header_line.substr(0, colon_pos);
// 		std::string value = header_line.substr(colon_pos + 1);
// 		key.erase(0, key.find_first_not_of(" \t"));
// 		key.erase(key.find_last_not_of(" \t") + 1);
// 		value.erase(0, value.find_first_not_of(" \t"));
// 		value.erase(value.find_last_not_of(" \t") + 1);
// 		std::transform(key.begin(), key.end(), key.begin(), ::tolower);

// 		std::cerr << "Header: '" << key << "' = '" << value << "'" << std::endl;  // ADD THIS
    
//     	std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    

// 		if (key == "content-length")
// 			content_length = std::stoi(value);
// 		else if (key == "content-type" && value.find("multipart/form-data") != std::string::npos)
// 		{
// 			size_t bpos = value.find("boundary=");
// 			if (bpos != std::string::npos)
// 				boundary = "--" + value.substr(bpos + 9);
// 		}

// 	// 	else if (key == "content-type")  // ADD THIS CHECK
//     // {
//     //     std::cerr << "Found Content-Type: '" << value << "'" << std::endl;
//     //     if (value.find("multipart/form-data") != std::string::npos)
//     //     {
//     //         std::cerr << "Is multipart!" << std::endl;
//     //         size_t bpos = value.find("boundary=");
//     //         if (bpos != std::string::npos)
//     //         {
//     //             boundary = "--" + value.substr(bpos + 9);
//     //             std::cerr << "Extracted boundary: '" << boundary << "'" << std::endl;
//     //         }
//     //         else
//     //             std::cerr << "No boundary= found in Content-Type" << std::endl;
//     //     }
//     // }
	
// 	// if (content_length > static_cast<int>(config->bodyLimit))
// 	// {
// 	// 	error_response(413, "payload too large");
// 	// 	return ;
// 	// }
	
// 	if (boundary.empty())
// 	{
// 		error_response(400, "bad request");
// 		return;
// 	}

// 	std::string body = http_request.substr(header_end + 4);

// 	// Check if route has upload path configured
// 	if (currentRoute->uploadPath.empty())
// 	{
// 		error_response(403, "Foridden");
// 		return;
// 	}

// 	std::vector<std::string> uploaded_files = parseMultipartBody(body, boundary, currentRoute->uploadPath);
	
// 	if (uploaded_files.empty())
// 	{
// 		error_response(400, "bad request");
// 		return;
// 	}
	
// 	std::string responseBody = "<html><head><title>Upload Success</title></head><body>";
// 	responseBody = responseBody + "<h1>Upload Successful</h1>";
// 	responseBody = responseBody + "<p>Uploaded " + std::to_string(uploaded_files.size()) + " file(s):</p>";
// 	responseBody = responseBody + "<ul>";
// 	for (const auto& filename : uploaded_files)
// 	{
// 		responseBody = responseBody + "<li>" + filename + "</li>";
// 	}
// 	responseBody = responseBody + "</ul>";
// 	responseBody = responseBody + "<a href=\"/\">Back to home</a>";
// 	responseBody = responseBody + "</body></html>";
	
// 	response =
// 		"HTTP/1.1 200 OK\r\n"
// 		"Content-Type: text/html\r\n"
// 		"Content-Length: " + std::to_string(responseBody.size()) + "\r\n"
// 		"\r\n" + responseBody;
// }

// }