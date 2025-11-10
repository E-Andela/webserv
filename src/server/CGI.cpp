#include "WebServer/CGI.hpp"
#include <unistd.h>
#include <stdexcept>

CGI::CGI(std::string cgi_path, std::string script_path, std::string request)
{
	_body = getBodyFromRequest(request);
	if (pipe(_pipeIn) == -1 || pipe(_pipeOut) == -1)
	{
		throw std::runtime_error("Failed to create pipes");
	}

	pid_t pid = fork();
	if (pid == -1)
	{
		throw std::runtime_error("Failed to fork");
	}
	if (pid == 0)
	{
		// Child process
		close(_pipeIn[1]);
		close(_pipeOut[0]);
		dup2(_pipeIn[0], STDIN_FILENO);
		dup2(_pipeOut[1], STDOUT_FILENO);
		close(_pipeIn[0]);
		close(_pipeOut[1]);

		// Set up environment variables here if needed
		execve(cgi_path.c_str(), NULL, _envp);
		exit(1); // execve failed
	}
	else
	{
		// Parent process
		close(_pipeIn[0]);
		close(_pipeOut[1]);
	}
}