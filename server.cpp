#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>

#define PORT "1212"

void print_address_info_result(struct addrinfo *result)
{
	char ipstr[INET6_ADDRSTRLEN];
	int i {0};
	for (struct addrinfo *p = result; p != NULL; p = p->ai_next)
	{
		std::cout << i << std::endl;
		i++;
		void *addr;
		const char *ipver;

		// Get the pointer to the address itself
		if (p->ai_family == AF_INET) // IPv4
		{
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		}
		else // IPv6
		{
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		// Convert the IP to a string and print it
		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		std::cout << "  " << ipver << ": " << ipstr << std::endl;
	}
}

int main()
{
	int status {0};
	struct addrinfo hints;
	struct addrinfo *result;
	struct addrinfo *p;

	std::memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	status = getaddrinfo(NULL, PORT, &hints, &result);
	if (status != 0)
	{
		std::cerr << "getaddrinfo() "<< gai_strerror(status) << std::endl;
		return (EXIT_FAILURE);
	}
	print_address_info_result(result);

	int serverFD {0};
	for (p = result; p != NULL; p = p->ai_next)
	{
		serverFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (serverFD == -1)
			continue;
		if (bind(serverFD, p->ai_addr, p->ai_addrlen) == 0)
		{
			std::cout << "Socket bound successfully." << std::endl;
			break;
		}
		close(serverFD);
	}
	if (p == NULL)
	{
		std::cerr << "Failed to bind socket" << std::endl;
		return (EXIT_FAILURE);
	}
	
	if (listen(serverFD, 1) == -1)
	{
		perror("listen");
		return (EXIT_FAILURE);
	}

	
	int clientfd = accept(serverFD, NULL, NULL);
	std::cout << "Connected to client\n";

	freeaddrinfo(result);
	close(serverFD);
	close(clientfd);
}