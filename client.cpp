#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>

#define PORT "1212"

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <server_address>" << std::endl;
		return EXIT_FAILURE;
	}
	std::cout << "Client started up" << std::endl;
	
	int status {0};
	struct addrinfo hints;
	struct addrinfo *result;
	struct addrinfo *p;
	std::memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	status = getaddrinfo(argv[1], PORT, &hints, &result);
	if (status != 0)
	{
		std::cerr << "getaddrinfo() "<< gai_strerror(status) << std::endl;
		return (EXIT_FAILURE);
	}

	int sockfd {0};
	for (p = result; p != NULL; p = p->ai_next)
	{
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockfd == -1)
			continue;
		
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			perror("connect");
			continue;
		}
		break;
	}
	if (p == NULL)
	{
		std::cerr << "Failed to connect" << std::endl;
		return (EXIT_FAILURE);
	}
}