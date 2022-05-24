// g++ -pthread client.cpp RobustIO.cpp -o client && g++ server.cpp RobustIO.cpp -o server
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include "RobustIO.h"

void *recv_thread(void *client_socket);
void *send_thread(void *client_socket);
char name[32];
char msg[512];
int main(int argc, char *argv[])
{
    struct addrinfo hints;
    struct addrinfo *addr;
    struct sockaddr_in *addrinfo;
    int rc;
    int sock;
    char buffer[512];
    int len;
    int userCount = 0;

    memset(&hints, 0, sizeof(hints));

    // TCP socket, IPv4/IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_ADDRCONFIG;

    rc = getaddrinfo("localhost", NULL, &hints, &addr);
    if (rc != 0)
    {
        printf("Hostname lookup failed: %s\n", gai_strerror(rc));
        exit(1);
    }

    addrinfo = (struct sockaddr_in *)addr->ai_addr;

    sock = socket(addrinfo->sin_family, addr->ai_socktype, addr->ai_protocol);
    if (sock < 0)
    {
        printf("Can't connect to server\n");
        exit(1);
    }

    addrinfo->sin_port = htons(55555);

    rc = connect(sock, (struct sockaddr *)addrinfo, addr->ai_addrlen);
    if (rc != 0)
    {
        printf("Connection failed\n");
        exit(1);
    }

    freeaddrinfo(addr);

        strcpy(name, argv[1]);
    if (userCount == 0){
        RobustIO::write_string(sock, name);
        userCount++;
    }
    pthread_t send_msg_thread;
    pthread_create(&send_msg_thread, NULL, send_thread, &sock);
    pthread_t recv_msg_thread;
    pthread_create(&recv_msg_thread, NULL, recv_thread, &sock);

    pthread_join(send_msg_thread, NULL);
    pthread_join(recv_msg_thread, NULL);
    close(sock);
}

void *send_thread(void *p_client_socket)
{
    int client_socket = *((int *)p_client_socket);
    char buffer[512];
    while (true){
        printf("-> ");
        std::cin.getline(msg, 512);
        printf("\n");
        printf("S: %s\n", msg);
        strcpy(buffer, name);
        strcat(buffer, ": ");
        strcat(buffer, msg);
        RobustIO::write_string(client_socket, buffer);
    }
    pthread_exit(NULL);
}

void *recv_thread(void *p_client_socket)
{
    int client_socket = *((int *)p_client_socket);
    char buffer[512];
    while (true){
        auto stuff = RobustIO::read_string(client_socket);
        strcpy(buffer, stuff.c_str());
        printf("%s\n", buffer);
        memset(&buffer, 0, sizeof(buffer));
    }
    pthread_exit(NULL);
}