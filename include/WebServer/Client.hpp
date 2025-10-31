#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include "Config/ServerConfig.hpp"

class Client
{
private:
	int _fd {};
	ServerConfig* _config {};
	std::string _request {};
	std::string _response {};
	std::string _body {};
	bool	_requestComplete {false};
	bool	_responseComplete {false};
	bool	_headersComplete {false};
	bool	_responseBuilt {false};
	bool	_chunkedTransfer {false};
	bool	_readingChunkSize {true};
	size_t _bytesSent {0};
	size_t	_contentLength {0};
	size_t	_chunkSize {0};
	
	void readChunkedBody();

public:
	Client(int fd, ServerConfig* config);
	
	int getFd() const;
	bool getRequestComplete() const;
	bool getResponseComplete() const;
	std::string getRequest() const;

	void buildRequest();
	void buildResponse();
	void sendResponse();
	int response(const std::string &response);
	void reset();
	ServerConfig* getConfig() const;


};

#endif