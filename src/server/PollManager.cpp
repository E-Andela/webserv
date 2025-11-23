#include "WebServer/PollManager.hpp"
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <bitset>

void PollManager::addListeningSocket(ListeningSocket* socket)
{
	int fd = socket->getFd();
	_sockets[fd] = socket;
	pollfd pfd;
	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pollfds.push_back(pfd);
}

void PollManager::addClient(const Client& client)
{
	int fd = client.getFd();
	_clients[fd] = std::make_shared<Client>(client);
	pollfd pfd;
	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pollfds.push_back(pfd);
}

void PollManager::acceptConnection(int fd)
{
	int clientFd = accept(fd, nullptr, nullptr);
	if (clientFd < 0)
	{
		return;
	}
	addClient(Client(clientFd, _sockets[fd]->getConfig()));
}

void PollManager::readClient(int fd)
{
	std::cout << "PollManager: readClient on fd " << fd << std::endl;
	auto it = _clients.find(fd);
	if (it != _clients.end())
	{
		Client& client = *(it->second);

		if (client.getFd() == fd)
		{
			if (!client.getRequestComplete())
				client.buildRequest();
			if (client.getRequestComplete())
			{
				client.buildResponse();
				if (client.getResponseBuilt())
				{
					registerForWrite(fd);
					// unregisterForRead(fd);
				}

				// if (!client.getCgiProcess().isCgiActive())
				// 	registerForWrite(fd);
				// unregisterForRead(fd);
			}
		}
		else {
			std::cout << "PollManager: readClient on CGI fd " << fd << std::endl;
			std::cout << "PID: " << client.getCgiProcess().getPid() << std::endl;
			client.cgiRead();
			if (client.getCgiProcess().isResponseComplete())
			{
				std::cout << "PollManager: CGI response complete for fd " << fd << std::endl;
				client.setResponse(client.getCgiProcess().getResponse());
				registerForWrite(client.getFd());
				unregisterForRead(fd);
			}
		}
	}
}

void PollManager::unregisterForRead(int fd)
{
	std::cout << "PollManager: Unregistering for POLLIN on fd " << fd << std::endl;
	for (size_t i = 0; i < _pollfds.size(); ++i)
	{
		if (_pollfds[i].fd == fd)
		{
			_pollfds[i].events &= ~POLLIN;
			return;
		}
	}
}

void PollManager::registerForRead(int fd)
{
	std::cout << "PollManager: Registering for POLLIN on fd " << fd << std::endl;
	for (size_t i = 0; i < _pollfds.size(); ++i)
	{
		if (_pollfds[i].fd == fd)
		{
			_pollfds[i].events |= POLLIN;
			return;
		}
	}
}

void PollManager::registerForWrite(int fd)
{
	std::cout << "PollManager: Registering for POLLOUT on fd " << fd << std::endl;
	for (size_t i = 0; i < _pollfds.size(); ++i)
	{
		if (_pollfds[i].fd == fd)
		{
			_pollfds[i].events |= POLLOUT;
			return;
		}
	}
}

void PollManager::unregisterForWrite(int fd)
{
	std::cout << "PollManager: Unregistering for POLLOUT on fd " << fd << std::endl;
	for (size_t i = 0; i < _pollfds.size(); ++i)
	{
		if (_pollfds[i].fd == fd)
		{
			_pollfds[i].events &= ~POLLOUT;
			return;
		}
	}
}

void PollManager::removeClient(int fd)
{
	_clients.erase(fd);

	for (auto it = _pollfds.begin(); it != _pollfds.end(); ++it)
	{
		if (it->fd == fd)
		{
			_pollfds.erase(it);
			break;
		}
	}
	close(fd);
}

void PollManager::handleAddQueue()
{
	std::vector<std::pair<int, std::shared_ptr<Client>>> newFds;

	for (auto it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getCgiProcess().isCgiActive())
		{
			std::queue<pollfd>& queue = it->second->getAddQueue();
			while (!queue.empty())
			{
				newFds.push_back({queue.front().fd, it->second});
				_pollfds.push_back(queue.front());
				queue.pop();
			}
		}
	}

	for (size_t i = 0; i < newFds.size(); ++i)
	{
		_clients[newFds[i].first] = newFds[i].second;
	}
}
// change to std::queue<int>& removeQueue = it->second->getRemoveQueue();
void PollManager::handleRemoveQueue()
{
	std::vector<int> fdsToRemove;

	for (auto it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getCgiProcess().isCgiActive() == false)
			continue;
		std::queue<int>& removeQueue = it->second->getRemoveQueue();
		while (!removeQueue.empty())
		{
			fdsToRemove.push_back(removeQueue.front());
			std::cout << "PollManager: Scheduling removal of fd " << removeQueue.front() << std::endl;
			removeQueue.pop();
		}
	}

	for (size_t i = 0; i < fdsToRemove.size(); ++i)
	{
		std::cout << "PollManager: Removing fd " << fdsToRemove[i] << std::endl;
		removeClient(fdsToRemove[i]);
	}
}

void PollManager::printPollFDs()
{
	std::cout << "Current pollfds:" << std::endl;
	for (size_t i = 0; i < _pollfds.size(); ++i)
	{
		std::cout << "fd: " << _pollfds[i].fd << std::endl;
	}
}

void PollManager::run()
{
	while (true)
	{
		printPollFDs();
		std::cout << "--------------------------" << std::endl;
		int events = poll(&_pollfds[0], _pollfds.size(), -1);
		printPollFDs();
		if (events > 0)
		{
			for (size_t i = 0; i < _pollfds.size(); ++i)
			{
				if (_pollfds[i].revents & POLLIN)
				{
					std::cout << "PollManager: POLLIN event on fd " << _pollfds[i].fd << std::endl;
					if (_sockets.count(_pollfds[i].fd))
					{
						acceptConnection(_pollfds[i].fd);
					}
					else if (_clients.count(_pollfds[i].fd))
					{
						try
						{
							readClient(_pollfds[i].fd);
						}
						catch(const std::exception& e)
						{
							removeClient(_pollfds[i].fd);
						}
					}
				}
				if (_pollfds[i].revents & POLLOUT)
				{
					std::cout << "PollManager: POLLOUT event on fd " << _pollfds[i].fd << std::endl;
					if (_clients.count(_pollfds[i].fd))
					{

						Client& client = *_clients[_pollfds[i].fd];
						if (_pollfds[i].fd == client.getFd())
						{
							client.sendResponse();
							if (client.getResponseComplete())
							{
								std::cout << "PollManager: Response complete for fd " << _pollfds[i].fd << std::endl;
								unregisterForWrite(_pollfds[i].fd);
								client.reset();
							}
						}
						else
						{
							std::cout << "PollManager: POLLOUT event on CGI fd " << _pollfds[i].fd << std::endl;
							client.cgiWrite();
						}
						
					}
				}
			}
		}
		else
		{
			break;
		}
		handleAddQueue();
		handleRemoveQueue();
	}
}