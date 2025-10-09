#pragma once

#include <vector>
#include "Config/ServerConfig.hpp"
#include "ListeningSocket.hpp"
#include "PollManager.hpp"

class WebServer
{
private:
	std::vector<ServerConfig> configs_;
	std::vector<ListeningSocket> sockets_;
	PollManager pollManager_;

public:
	WebServer(const std::vector<ServerConfig>& configs);

	void initSockets();
	void initPollManager();
	void run();
};