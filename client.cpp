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
}
