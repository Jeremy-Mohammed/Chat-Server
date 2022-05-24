#include "RobustIO.h"


int RobustIO::readn(int fd, char* buffer, int count) {
    char* ptr = buffer;
	int left = count;
    int n;

	while(left > 0) {
		n = read(fd, ptr, left);
		if(n == 0)
			break;
		if(n < 0) {
			if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
				n = 0;
			} else {
				return(-1);
			}
		}
		left -= n;
		ptr += n;
	}
	return (count-left);
}


int RobustIO::writen(int fd, char* buffer, int count) {
    char* ptr = buffer;
	int left = count;
    int n;

	while(left > 0) {
		n = write(fd, ptr, left);
		if(n < 0) {
			if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
				n = 0;
			} else {
				return(-1);
			}
		}
		left -= n;
		ptr += n;
	}
	return (count-left);
}

int RobustIO::write_string(int fd, std::string s) {
    int len = s.length() + 1;
    int buf = htonl(len);

    writen(fd, (char*) &buf, sizeof(len));

    return writen(fd, (char*) s.c_str(), len);
}

std::string RobustIO::read_string(int fd) {
    int len;

    readn(fd, (char*) &len, 4);
    len = ntohl(len);

    char* buffer = new char[len];
    readn(fd, buffer, len);
    
    std::string s = buffer;
    return s;
}