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
