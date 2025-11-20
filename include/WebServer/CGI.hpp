#ifndef CGI_HPP
# define CGI_HPP

#include <map>
#include <string>
#include <queue>
#include <poll.h>
#include <unordered_map>

struct FDRegistration {
	int fd;
	short events;
	void* owner;
};

class CGI
{
private:
	int _pipeIn[2];
	int _pipeOut[2];
	size_t _bytesWritten {0};
	size_t _bytesRead {0};
	std::string _response;
	std::string	_body;
	std::string _headers;
	std::queue<pollfd> _pendingFDs;
	std::queue<int> _removeFDs;
	char** _env;
	std::unordered_map<std::string, std::string> _envMap;
	bool _responseComplete {false};

	std::string getBodyFromRequest(const std::string& request);
	std::string getHeadersFromRequest(const std::string& request);
	char** createCGIEnv();
	void createEnvironmentMap(std::string method, std::string query_string);
	std::unordered_map<std::string, std::string> makeKeyValues(std::string headers);

public:
	CGI(std::string cgi_path, std::string script_path, std::string request, std::string method, std::string query_string);
	CGI();
	std::queue<pollfd>& getPendingFDs();
	std::queue<int>& getRemoveFDs();
	void writePipe();
	void readPipe();
	bool isResponseComplete() const;
	std::string getResponse() const;
};

#endif