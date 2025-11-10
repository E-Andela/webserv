#ifndef CGI_HPP
# define CGI_HPP

#include <map>
#include <string>
#include <queue>
#include <poll.h>

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
	std::string _response;
	std::string	_body;
	std::queue<pollfd> _pendingFDs;
	std::queue<int> _removeFDs;
	char** _envp;

public:
	CGI(std::string cgi_path, std::string script_path);
	std::queue<pollfd>& getPendingFDs();
	std::queue<int>& getRemoveFDs();
	void readPipe();
	void writePipe();
};

#endif