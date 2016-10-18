#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>


int main(int argc, char **argv)
{
    int s=socket(AF_INET, SOCK_DGRAM, 0);
    if(s<0) {
	perror("socket");
	return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(256);

    struct hostent *hp = gethostbyname("192.168.0.2");
    if (!hp) {
	perror("hostname");
	return 1;
    }
    memcpy((void *) &addr.sin_addr, hp->h_addr_list[0], hp->h_length);
    if(connect(s,reinterpret_cast<struct sockaddr*>(&addr),sizeof(addr))<0) {
	perror("connect");
	return 1;
    }

    send(s,"xxx",3,0);

    for(;;) {
	union {
	    char buffer[1000];
	    struct {
		uint32_t counter;
		uint32_t encoder;
	    };
	} d;
	int  len=recv(s,&d,sizeof(d),0);
	std::cout << "ack " << len << " " << d.counter << " " << d.encoder << std::endl;
    }
}
