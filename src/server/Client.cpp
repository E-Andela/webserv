#include "WebServer/Client.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdexcept>
#include <iostream>
#include "WebServer/CGI.hpp"

Client::Client(int fd, ServerConfig* config) : _fd {fd}, _config {config}, _requestComplete {false}, _responseComplete {false}
{

}

int Client::getFd() const
{
	return _fd;
}

bool Client::getRequestComplete() const
{
	return _requestComplete;
}

bool Client::getResponseComplete() const
{
	return _responseComplete;
}

std::string Client::getRequest() const
{
	return _request;
}

void Client::setResponse(std::string response)
{
	_response = response;
}

void Client::readChunkedBody()
{
	size_t headerEnd = _request.find("\r\n\r\n");
	std::string bodyPart = _request.substr(headerEnd + 4);

	while (true)
	{
		if (_readingChunkSize)
		{
			size_t pos = bodyPart.find("\r\n");
			if (pos == std::string::npos)
				break;
			
			std::string sizeStr = bodyPart.substr(0, pos);
			_chunkSize = std::strtoul(sizeStr.c_str(), NULL, 16);
			bodyPart.erase(0, pos + 2);

			if (_chunkSize == 0)
			{
				size_t endChunk = bodyPart.find("\r\n");
				if (endChunk != std::string::npos)
				{
					_requestComplete = true;
				}
				break;
			}

			_readingChunkSize = false;
		}
		if (!_readingChunkSize)
		{
			if (bodyPart.size() < _chunkSize + 2)
				break;

			_body.append(bodyPart, 0, _chunkSize);
			bodyPart.erase(0, _chunkSize + 2);

			_readingChunkSize = true;
		}
	}
}

/**
 * @brief Reads data from the client socket and builds the HTTP request.
 *
 * This function reads incoming data from the client file descriptor (_fd)
 * and appends it to the internal request buffer (_request). It checks for
 * the end of the HTTP headers (marked by "\r\n\r\n") to determine if the
 * request headers are complete and sets the _requestComplete flag accordingly.
 *
 * @note The function is incomplete as it does not handle or expect an HTTP request body.
 *       It only processes the headers and does not support requests with bodies (e.g., POST).
 *
 * @throws std::runtime_error if the client disconnects (recv returns 0).
 */
void Client::buildRequest()
{
	char buf[1024];
	int res = recv(_fd, buf, sizeof(buf), 0);
	if (res <= 0)
	{
		throw std::runtime_error("Client disconnected");
	}

	_request.append(buf, res);

	if (!_headersComplete)
	{
		size_t headerEnd = _request.find("\r\n\r\n");
		if (headerEnd != std::string::npos)
		{
			_headersComplete = true;
			std::string headers = _request.substr(0, headerEnd + 4);
	
			size_t pos = headers.find("Content-Length:");
			if (pos != std::string::npos)
			{
				_contentLength = std::stoi(headers.substr(pos + 15));
			}
			else
			{
				pos = headers.find("Transfer-Encoding: chunked");
				if (pos != std::string::npos)
				{
					_chunkedTransfer = true;
				}
			}
		}
	}
	if (_headersComplete)
	{
		if (!_chunkedTransfer)
		{
			size_t headerEnd = _request.find("\r\n\r\n");
			size_t bodySize = _request.size() - (headerEnd + 4);

			if (bodySize >= _contentLength)
			{
				std::cout << "Client::buildRequest() - Request complete" << std::endl;
				_requestComplete = true;
			}
		}
		else
		{
			// Handle chunked transfer encoding
			readChunkedBody();
			if (_requestComplete)
			{
				size_t headerEnd = _request.find("\r\n\r\n");
				_request.replace(headerEnd + 4, _body.size(), _body);
			}
		}
		
	}
}


void Client::buildResponse()
{
	std::cout << "Client::buildResponse()" << std::endl;
    // _response = "HTTP/1.1 200 OK\r\n";
    // _response += "Content-Length: 13\r\n";
    // _response += "Content-Type: text/plain\r\n";
    // _response += "\r\n";
    // _response += "Hello, world!";
	// _responseBuilt = true;


	if (!_responseBuilt)
	{
		std::cerr << _request << std::endl;
		ParseHTTP parser;
		parser.setClient(this);
		parser.setConfig(getConfig());
		parser.parse_http_request();

		if (!_cgiProcess.isCgiActive())
		{
			setResponse(parser.getResponse());
			_responseBuilt = true;
			std::cout << "Response: " << std::endl;
			std::cout << "-------------------------------" << std::endl;
			std::cout << _response << std::endl;
			std::cout << "-------------------------------" << std::endl;
		}
		else
		{
			if (_cgiProcess.isResponseComplete())
			{
				setResponse(_cgiProcess.getResponse());
				_responseBuilt = true;
				std::cout << "Response: " << std::endl;
				std::cout << "-------------------------------" << std::endl;
				std::cout << _response << std::endl;
				std::cout << "-------------------------------" << std::endl;
			}
		}
	}
}

void Client::sendResponse()
{
	buildResponse();
	if (_responseBuilt)
	{
		std::cout << "Client::sendResponse()" << std::endl;
		std::cout << "-------------------------------" << std::endl;

		size_t sent = send(_fd, _response.c_str() + _bytesSent, _response.size() - _bytesSent, 0);
		if (sent <= 0)
		{
			throw std::runtime_error("Client disconnected");
		}

		_bytesSent += sent;

		std::cout << "sent: " << sent << " bytes, total: " << _bytesSent << "/" << _response.size() << std::endl;
		if (_bytesSent >= _response.size())
			_responseComplete = true;
		std::cout << "-------------------------------" << std::endl;
	}
}

void Client::cgiRead()
{
	_cgiProcess.readPipe();
}

void Client::cgiWrite()
{
	_cgiProcess.writePipe();
}

void Client::reset()
{
	_response.clear();
	_request.clear();
	_body.clear();
	_requestComplete = false;
	_responseComplete = false;
	_headersComplete = false;
	_responseBuilt = false;
	_chunkedTransfer = false;
	_readingChunkSize = true;
	_contentLength = 0;
	_bytesSent = 0;
	_chunkSize = 0;
}

ServerConfig* Client::getConfig() const
{
	return _config;
}

std::queue<pollfd>& Client::getAddQueue()
{
	return _cgiProcess.getPendingFDs();
}

std::queue<int>& Client::getRemoveQueue()
{
	return _cgiProcess.getRemoveFDs();
}

void Client::setCgiProcess(CGI cgi)
{
	_cgiProcess = cgi;
}

void Client::createCgiProcess(std::string cgi_path, std::string script_path, std::string method, std::string query_string)
{
	_cgiProcess = CGI(cgi_path, script_path, _request, method, query_string);
}

CGI& Client::getCgiProcess()
{
	return _cgiProcess;
}

