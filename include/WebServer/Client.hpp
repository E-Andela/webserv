#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

class Client
{
private:
	int _fd {};
	std::string _request {};
	std::string _response {};
	bool	_requestComplete {false};
	bool	_responseComplete {false};

public:
	Client(int fd);
	
	int getFd() const;
	bool getRequestComplete() const;
	bool getResponseComplete() const;
	std::string& getRequest() const;

	void buildRequest();
	void buildResponse();
	void sendResponse();
	int response(const std::string &response);
	void reset();


};

#endif