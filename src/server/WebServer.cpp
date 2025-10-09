#include "WebServer/WebServer.hpp"
#include <iostream>

WebServer::WebServer(const std::vector<ServerConfig>& configs) : configs_ (configs)
{
	for (std::vector<ServerConfig>::iterator it = configs_.begin(); it != configs_.end(); ++it)
	{
		std::cout << "port: " << it->port << std::endl;
	}
}

void WebServer::initSockets()
{
	for (std::vector<ServerConfig>::iterator it = configs_.begin(); it != configs_.end(); ++it)
	{
		int port = it->port;
		ListeningSocket* existing = nullptr;

		for (std::vector<ListeningSocket>::iterator sock = sockets_.begin(); sock != sockets_.end(); ++sock)
		{
			if (sock->getPort() == port)
			{
				existing = &*sock;
				break;
			}
		}

		if (existing)
		{
			existing->addConfig(&*it);
		}
		else
		{
			ListeningSocket newSock(port, "0.0.0.0");
			newSock.addConfig(&*it);
			sockets_.push_back(newSock);
		}
	}
}

void WebServer::initPollManager()
{
	for (std::vector<ListeningSocket>::iterator it = sockets_.begin(); it != sockets_.end(); ++it)
	{
		pollManager_.addListeningSocket(&*it);
	}
}

void WebServer::run()
{
	pollManager_.run();
}