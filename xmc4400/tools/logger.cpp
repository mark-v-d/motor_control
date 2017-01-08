#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>


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

    std::cout << "#   seq    encoder adc0 adc1 adc2 adc3 crc\n";

    int count=9000;
    if(argc>1)
	count=atoi(argv[1]);
    while(--count>0) {
	union {
	    char buffer[1000];
	    struct {
		uint32_t counter;
		float adc[4];
		uint32_t encoder;
		float Irotor[2];
		float Vrotor[2];
		float angle;
		float output[3];
		uint8_t enc_raw[9];
	    };
	} d;
	int  len=recv(s,&d,sizeof(d),0);
	std::cout << std::dec << len << " "
	    << d.counter << " " << std::setw(6) << d.encoder 
	    << std::setprecision(3) << std::fixed;
	for(int i=0;i<4;i++)
	    std::cout << " " << std::setw(8) << d.adc[i];
	std::cout   << " " << d.Vrotor[0] << " " << d.Vrotor[1]
		    << " " << d.Irotor[0] << " " << d.Irotor[1];

	uint16_t checksum=0;
	for(int i=0;i<sizeof(d.enc_raw);i++) {
	    std::cout << " " << std::setw(3) << int(d.enc_raw[i]);
	    checksum^=d.enc_raw[i];
	}
	std::cout <<  " " << checksum << " " << d.angle ;
	for(int i=0;i<3;i++)
	    std::cout << " " << std::setw(8) << d.output[i];
	std::cout << std::endl;
    }
}
