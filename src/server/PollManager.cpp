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
	std::cout << "PollManager::readClient() - Reading from client fd: " << fd << std::endl;
	auto it = _clients.find(fd);
	if (it != _clients.end())
	{
		Client& client = *(it->second);

		if (client.getFd() == fd)
		{
			if (!client.getRequestComplete())
				client.buildRequest();
			if (client.getRequestComplete())
				registerForWrite(fd);
		}
		else {
			client.cgiRead();
			if (client.getCgiProcess().isResponseComplete())
			{
				client.setResponse(client.getCgiProcess().getResponse());
				registerForWrite(fd);
			}
		}

	}
}

void PollManager::unregisterForRead(int fd)
{
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
			std::queue<pollfd> queue = it->second->getAddQueue();
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
		std::queue<int> removeQueue = it->second->getRemoveQueue();
		while (!removeQueue.empty())
		{
			fdsToRemove.push_back(removeQueue.front());
			removeQueue.pop();
		}
	}

	for (size_t i = 0; i < fdsToRemove.size(); ++i)
	{
		removeClient(fdsToRemove[i]);
	}
}

void PollManager::run()
{
	while (true)
	{
		int events = poll(&_pollfds[0], _pollfds.size(), -1);
		if (events > 0)
		{
			for (size_t i = 0; i < _pollfds.size(); ++i)
			{
				if (_pollfds[i].revents & POLLIN)
				{
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
							std::cout << "client removed" << std::endl;
							removeClient(_pollfds[i].fd);
						}
					}
				}
				if (_pollfds[i].revents & POLLOUT)
				{
					if (_clients.count(_pollfds[i].fd))
					{
						Client& client = *_clients[_pollfds[i].fd];
						if (_pollfds[i].fd == client.getFd())
						{
							client.sendResponse();
							if (client.getResponseComplete())
							{
								unregisterForWrite(_pollfds[i].fd);
								client.reset();
							}
						}
						else
						{
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