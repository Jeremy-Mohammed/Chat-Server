#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "RobustIO.h"
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/select.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <list>
struct memory
{
	char buff[100];
	int status, pid1, pid2;
};

int main(int argc, char **argv)
{
	int sock, conn; // need socket for sercer and one for new connections
	int i;
	int rc;
	struct sockaddr address;
	socklen_t addrLength = sizeof(address);
	struct addrinfo hints;
	struct addrinfo *addr;
	char buffer[512];
	int len;
	std::list<std::string> history = {};
	// Clear the address hints structure
	memset(&hints, 0, sizeof(hints));

	hints.ai_socktype = SOCK_STREAM;			 // TCP
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; // IPv4/6, socket is for binding
	// Get address info for local host (still the server)
	if ((rc = getaddrinfo(NULL, "55555", &hints, &addr)))
	{
		printf("host name lookup failed: %s\n", gai_strerror(rc));
		exit(1);
	}

	// Create a socket
	sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (sock < 0)
	{
		printf("Can't create socket\n");
		exit(1);
	}

	// Set the socket for address reuse, so it doesn't complain about
	// other servers on the machine.
	i = 1;
	// set options for user (SO_REUSEADDR)
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));

	// Bind the socket
	rc = bind(sock, addr->ai_addr, addr->ai_addrlen);
	if (rc < 0)
	{
		printf("Can't bind socket\n");
		exit(1);
	}

	// Clear up the address data
	freeaddrinfo(addr);

	// Listen for new connections, wait on up to five of them , listen to socket up to 5 connections
	rc = listen(sock, 5);
	if (rc < 0)
	{
		printf("listen failed\n");
		exit(1);
	}

	fd_set master; // master file descriptor list
	fd_set read_fds;

	int client, length, fdmax;
	FD_SET(sock, &master);
	fdmax = sock;
	bool running = true;
	std::string nam;
	for (;;){
		read_fds = master;
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(4);
		}
		for (int i = 0; i <= fdmax; i++){
			if (FD_ISSET(i, &read_fds)){
				if (i == sock){
					int newfd = accept(sock, (struct sockaddr *)&address, &addrLength);
					if (newfd == -1){
						perror("err");
					}
					else{
						FD_SET(newfd, &master);
						if (newfd > fdmax){
							fdmax = newfd;
						}
					}
					if (newfd >= 5){
						for (std::list<std::string>::const_iterator i = history.begin(); i != history.end(); ++i){
							RobustIO::write_string(newfd, i->c_str());
						}
					}
					std::string wel = "Welcome to chat room!";
					RobustIO::write_string(newfd, wel.c_str());
				}
				else{
					std::string s = RobustIO::read_string(i);
					std::string newString = s + "\n";

					if (history.size() >= 12){
						history.pop_back();
					}
					history.push_back(newString);

					std::cout << s << std::endl;
					if (s.empty()){
						if (s == ""){
							printf("Socket %d disconnected!\n", i);
						}
						else{
							perror("Receive");
						}
						close(i);
						FD_CLR(i, &master);
					}
					else{
						for (int j = 0; j <= fdmax; j++){
							if (FD_ISSET(j, &master)){
								if (j != sock && j != i){
									RobustIO::write_string(j, s.c_str());
								}
							}
						}
					}
				}
			}
		}
	}
}