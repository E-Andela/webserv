#include "WebServer/ListeningSocket.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdexcept>
#include <unistd.h>
#include <cstring>
#include <iostream>

ListeningSocket::ListeningSocket(int port, const std::string &address)
    : _fd{-1}
	, _port{port}
	, address_{address}
{
	struct addrinfo hints, *res, *p;

    std::memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	std::string portStr = std::to_string(port);

	if (getaddrinfo(address_.c_str(), portStr.c_str(), &hints, &res) == -1)
	{
		throw std::runtime_error("getaddrinfo() failed");
	}

	for (p = res; p != NULL; p = p->ai_next) {
        _fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (_fd == -1)
            continue;

        int opt = 1;
        setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        if (bind(_fd, p->ai_addr, p->ai_addrlen) == 0) {
            if (listen(_fd, SOMAXCONN) == 0)
                break;
        }

        close(_fd);
        _fd = -1;
    }

    freeaddrinfo(res);

    if (!p) {
        throw std::runtime_error("Failed to bind and listen on socket");
    }
	
}

ListeningSocket::~ListeningSocket()
{
    // if (_fd != -1) {
    //     close(_fd);
    // }
	std::cout << "destructor" << std::endl;
}

int ListeningSocket::getFd() const
{
    return _fd;
}

int ListeningSocket::getPort() const
{
	return _port;
}

void ListeningSocket::addConfig(ServerConfig* config)
{
	configs_.push_back(config);
}
