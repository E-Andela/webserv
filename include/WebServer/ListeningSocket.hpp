#ifndef LISTENING_SOCKET_HPP
# define LISTENING_SOCKET_HPP

# include <string>
# include "Config/ServerConfig.hpp"

class ListeningSocket {

private:
	int _fd;
	int _port;
	std::string address_;
	std::vector<ServerConfig*> configs_;

public:
	ListeningSocket(int port, const std::string& address);
	~ListeningSocket();
	
	void addConfig(ServerConfig* config);
	int getFd() const;
	int getPort() const;
	ServerConfig* getConfig() const;
};

#endif