// #ifndef CLIENT_HPP
// # define CLIENT_HPP

// # include <string>
// # include "Config/ServerConfig.hpp"
// # include "ParseHTTP.hpp"

// class Client
// {
// private:
// 	int _fd {};
// 	std::string _request {};
// 	std::string _response {};
// 	bool	_requestComplete {false};
// 	bool	_responseComplete {false};
// 	ServerConfig* _serverConfig{};
// 	bool _headersComplete {false};
// 	size_t _contentLength {0};

// public:
// 	Client(int fd, ServerConfig* config);
	
// 	int getFd() const;
// 	bool getRequestComplete() const;
// 	bool getResponseComplete() const;
// 	std::string getRequest() const;
// 	void setResponse(std::string response); 

// 	void buildRequest();
// 	void buildResponse();
// 	void sendResponse();
// 	int response(const std::string &response);
// 	void reset();
// 	ServerConfig* getServerConfig() const;


// };

// #endif