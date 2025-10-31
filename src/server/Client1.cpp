// #include "WebServer/Client1.hpp"
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <stdexcept>
// #include <iostream>

// Client::Client(int fd, ServerConfig* config) : _fd {fd}, _requestComplete {false}, _responseComplete {false}, _serverConfig {config}
// {

// }

// int Client::getFd() const
// {
// 	return _fd;
// }

// bool Client::getRequestComplete() const
// {
// 	return _requestComplete;
// }

// bool Client::getResponseComplete() const
// {
// 	return _responseComplete;
// }

// std::string Client::getRequest() const
// {
// 	return _request;
// }

// void Client::setResponse(std::string response)
// {
// 	_response = response;
// }

// /**
//  * @brief Reads data from the client socket and builds the HTTP request.
//  *
//  * This function reads incoming data from the client file descriptor (_fd)
//  * and appends it to the internal request buffer (_request). It checks for
//  * the end of the HTTP headers (marked by "\r\n\r\n") to determine if the
//  * request headers are complete and sets the _requestComplete flag accordingly.
//  *
//  * @note The function is incomplete as it does not handle or expect an HTTP request body.
//  *       It only processes the headers and does not support requests with bodies (e.g., POST).
//  *
//  * @throws std::runtime_error if the client disconnects (recv returns 0).
//  */


// void Client::buildRequest()
// {
//     char buf[1024];
//     int res = recv(_fd, buf, sizeof(buf), 0);

//     if (res <= 0)
//     {
//         throw std::runtime_error("Client disconnected");
//     }   

// 	_request.append(buf, res);    
// 	if (!_headersComplete)
//     {
//         size_t headerEnd = _request.find("\r\n\r\n");
//         if (headerEnd != std::string::npos)
//             _headersComplete = true;       
// 		std::string headers = _request.substr(0, headerEnd + 4);        
// 		size_t pos = headers.find("Content-Length:");
//         if (pos != std::string::npos)
//         {
//             _contentLength = std::stoi(headers.substr(pos + 15));
//         }
//     }    
// 	if (_headersComplete)
//     {
//         size_t headerEnd = _request.find("\r\n\r\n");
//         size_t bodySize = _request.size() - (headerEnd + 4);        
// 		if (bodySize >= _contentLength)
//         {
//             _requestComplete = true;
//         }
//     }
// }


// void Client::buildResponse()
// {
//     // _response = "HTTP/1.1 200 OK\r\n";
//     // _response += "Content-Length: 13\r\n";
//     // _response += "Content-Type: text/plain\r\n";
//     // _response += "\r\n";
//     // _response += "Hello, world!";

// 	std::cerr << _request << std::endl;
// 	ParseHTTP parser;
// 	parser.setClient(this);
// 	parser.setConfig(getServerConfig());
// 	parser.parse_http_request();


// 	setResponse(parser.getResponse());
// }

// void Client::sendResponse()
// {
// 	buildResponse();
// 	std::cout << "Client::sendResponse() " << std::endl;
// 	std::cout << "-------------------------------" << std::endl;
// 	int sent = send(_fd, _response.c_str(), _response.size(), 0);
// 	std::cout << "sent: " << sent << std::endl;
// 	_responseComplete = true;
// 	std::cout << "-------------------------------" << std::endl;
// }

// void Client::reset()
// {
// 	_response.clear();
// 	_request.clear();
// 	_requestComplete = false;
// 	_responseComplete = false;
// 	_headersComplete = false;
// 	_contentLength = 0;
// }

// ServerConfig* Client::getServerConfig() const
// {
// 	return _serverConfig;
// }

// // std::string Client::request()
// // {
// // 	std::cout << "Client::request() " << std::endl;
// // 	std::cout << "-------------------------------" << std::endl;
// // 	char buf[1024];
// // 	int res = recv(_fd, buf, sizeof(buf), 0);
// // 	if (res < 0)
// // 	{
// // 		throw std::runtime_error("Failed to receive data from client");
// // 	}
// // 	else if (res == 0)
// // 	{
// // 		return "";
// // 	}
	
// // 	std::cout << "Received " << res << " bytes from client" << std::endl;
// // 	std::cout << "Buffer: " << buf << std::endl;
// // 	// _request = _buffer;
// // 	std::cout << "-------------------------------" << std::endl;
// // 	return _request;
// // }

