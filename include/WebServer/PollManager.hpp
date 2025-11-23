#ifndef POLLMANAGER_HPP
# define POLLMANAGER_HPP

#include <map>
#include <memory>
#include "ListeningSocket.hpp"
#include "Client.hpp"
#include <vector>
#include <poll.h>

class PollManager
{
private:
	std::map<int, ListeningSocket*> _sockets;
	std::map<int, std::shared_ptr<Client>> _clients;
	std::vector<pollfd> _pollfds {};

public:
	void addListeningSocket(ListeningSocket* socket);
	void unregisterForRead(int fd);
	void registerForRead(int fd);
	void registerForWrite(int fd);
	void unregisterForWrite(int fd);
	void addClient(const Client& client);
	void acceptConnection(int fd);
	void readClient(int fd);
	void run();
	void removeClient(int fd);
	void handleAddQueue();
	void handleRemoveQueue();
	void printPollFDs();
};

#endif