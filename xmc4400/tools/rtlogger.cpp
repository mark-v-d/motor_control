#include <signal.h>
#include <pthread.h>
#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <chrono>

#include <unistd.h>
#include <sys/syscall.h> 
/* Ugly, but .... */
#define gettid() syscall(__NR_gettid)
#define sigev_notify_thread_id _sigev_un._tid


std::ostream &operator << (std::ostream &s,struct timespec const &t)
{
    double time=t.tv_sec+1e-9*t.tv_nsec;
    return s << time << "s";
}

struct sync_t {
    uint32_t tx_sec;
    uint32_t tx_nsec;
    uint32_t rx_sec;
    uint32_t rx_nsec;
    uint32_t timer; 
    float integrator;

    sync_t(void) {}
    sync_t(uint8_t *p) {
	sync_t *src=reinterpret_cast<sync_t*>(p);
	*this=*src;
    }
};
std::array<sync_t,45000> loggin;

std::ostream &operator << (std::ostream &s,struct sync_t const &t)
{
    double tx=t.tx_sec+1e-9*t.tx_nsec;
    double rx=t.rx_sec+1e-9*t.rx_nsec;
    return s << std::fixed << std::setprecision(9) 
	<< tx << " " << rx << " " << t.timer << " "
	<< t.integrator << std::endl;
}


void periodic(int i)
{
    std::cout << "periodic\n";
}

void *sender(void*)
{
    // Open socket and connect to first board
    int s=socket(AF_INET, SOCK_DGRAM, 0);
    if(s<0) {
	perror("socket");
	return NULL;
    }

    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(3);

    struct hostent *hp = gethostbyname("192.168.0.5");
    if (!hp) {
	perror("hostname");
	return NULL;
    }
    memcpy((void *) &addr.sin_addr, hp->h_addr_list[0], hp->h_length);
    if(connect(s,reinterpret_cast<struct sockaddr*>(&addr),sizeof(addr))<0) {
	perror("connect");
	return NULL;
    }

    // Set real-time scheduling
    struct sched_param param;
    param.sched_priority=sched_get_priority_max(SCHED_FIFO);
    if(pthread_setschedparam(pthread_self(), SCHED_FIFO, &param))
	throw "pthread_setschedparam";

    // Create timer
    struct sigevent ev;
    ev.sigev_notify=SIGEV_THREAD_ID;
    ev.sigev_signo=SIGALRM;
    ev.sigev_notify_thread_id=gettid();
    timer_t tid;
    if(timer_create(CLOCK_MONOTONIC, &ev, &tid)) {
	perror("timer_create");
	throw "timer_create";
    }


    // Start 4500Hz timer
    struct itimerspec its;
    its.it_value.tv_sec=0;
    its.it_value.tv_nsec=222222;	// almost 4500Hz
    its.it_interval.tv_sec=0;
    its.it_interval.tv_nsec=222222;
    std::cout << "# value=" << its.it_value 
	<< " interval=" << its.it_interval << std::endl;
    if(timer_settime(tid, 0, &its, NULL)) { 
	perror("timer_settime");
    }


    // send packets for 10 sec
    for(int i=0;i<loggin.size();i++) { 
	int sigs;
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);
	if(sigwait(&sigset, &sigs)<0) {
	    perror("sigwait");
	    return NULL;
	}
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC,&now);
	struct itimerspec nxt;
	timer_gettime(tid, &nxt);
	struct {
	    uint32_t now_sec;
	    uint32_t now_nsec;
	    uint32_t next_sec;
	    uint32_t next_nsec;
	} n;
	n.next_sec=now.tv_sec;
	n.next_nsec=now.tv_nsec+nxt.it_value.tv_nsec;
	if(n.next_nsec>1'000'000'000) {
	    n.next_sec++;
	    n.next_nsec-=1'000'000'000;
	}
	n.now_sec=now.tv_sec;
	n.now_nsec=now.tv_nsec;
	send(s,&n,sizeof(n),0);
	uint8_t buffer[1024];
	recv(s,buffer,sizeof(buffer),0);
	loggin[i]=buffer;
    }

    struct timespec tp;
    if(clock_gettime(CLOCK_MONOTONIC, &tp))
	throw "clock_gettime";
    std::cout << "# " << timer_getoverrun(tid) << " " << tp << " ok\n";
    for(auto x:loggin)
	std::cout << x;
}

int main(int argc, char **argv)
{
    // Create thread running at 4500Hz
    pthread_attr_t attr;
    if(pthread_attr_init(&attr))
	throw "pthread_attr_init";
    pthread_t thread;
    if(pthread_create(&thread, &attr, sender, NULL))
	throw "pthread_create";

    pthread_join(thread,NULL);
    return 0;


#if 0
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
#endif
}
