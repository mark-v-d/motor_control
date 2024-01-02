/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 */

#include <sys/io.h>

#include <limits.h>
#include <sched.h>
#include <pthread.h>
#include <sys/mman.h>

#include "drive_io.h"

#include <iostream>
#include <fstream>
#include <complex>
#include <chrono>
using namespace std::complex_literals;
using namespace std::chrono_literals;

std::array<uint8_t,6> mac{0xc2, 0x00, 0x03, 0x08, 0x10, 0xc0}; // SPINDLE
//std::array<uint8_t,6> mac{0xc2, 0x00, 0x85, 0x0c, 0x10, 0xc0}; // X
//std::array<uint8_t,6> mac{0xc2, 0x00, 0x8d, 0x11, 0x11, 0xc0}; // Z
//std::array<uint8_t,6> mac{0xc2, 0x00, 0x86, 0x05, 0x10, 0xc0}; // T
constexpr std::array<uint8_t,4> ip{192,168,0,6};

using timebase_t=std::chrono::duration<int,std::ratio<1,4500>>;

std::complex<double> current=0.1i;
struct sync_I_t:public sync_t {
    std::complex<float> Iset{0,0};
    std::complex<float> limit{0.44,0.75};
    float Vset=0, Imax=0, Vdelta=0;
    int at_speed;
};
std::vector<sync_I_t> table;

std::ostream &operator<<(std::ostream &s, sync_I_t const &d) {
    s	<< sync_t(d)
	<< " " << real(d.Iset) << " " << imag(d.Iset)	// 28, 29
	<< " " << real(d.limit) << " " << imag(d.limit)	// 30,31
	<< " " << d.Vset				// 32
	<< " " << d.Imax				// 33
	<< " " << d.Vdelta				// 34
	<< " " << d.at_speed;				// 35
    return s;
}

raw_socket skt("eth1");


class speed_voltage_t {
    float Vcurrent;
    float Imax;
    float Vdelta;
    float Vmeasured;
    float gain=0.1;
public:
    void set(float Vd,float Im) { Imax=Im; Vdelta=Vd; }

    auto compute(float Vset, float Vservo, float Vrotor) {
	Vmeasured+=gain*(Vrotor*Vservo-Vmeasured);
	float Iset=Vcurrent<0? -Imax:Imax;
	float duty=std::abs(Vcurrent/Vservo);
	if(Vset>Vcurrent) {
	    if(Vcurrent<-1) {
		// Decelerate
		Vcurrent=Vmeasured;
		Iset=0;
		duty=0.75;
	    } else {
		// Accelerate
		Vcurrent+=Vdelta;
		Iset=Imax;
		duty=std::abs(Vcurrent/Vservo);
	    }
	    if(Vcurrent>Vset)
		Vcurrent=Vset;
	} else if(Vset<Vcurrent) {
	    if(Vcurrent>1) {
		// Decelerate
		Vcurrent=Vmeasured;
		Iset=0;
		duty=0.75;
	    } else {
		// Accelerate
		Vcurrent-=Vdelta;
		Iset=-Imax;
		duty=std::abs(Vcurrent/Vservo);
	    }
	}

	duty=std::min(0.75f,duty);
	duty=std::max(0.0f,duty);


	bool at_speed=std::abs(Vmeasured-Vset)<1.0;

	if(Imax==0)
	    duty=0.75;

	return std::make_tuple(
	    std::complex<float>{0,Iset},
	    std::complex<float>{0.44,duty},
	    at_speed
	);
    }
} sc;



void *rt_thread(void *data)
{
    timebase_t time=0s;
    struct timespec ts;
    if(clock_gettime(CLOCK_MONOTONIC, &ts)) {
	perror("clock_gettime");
	return NULL;
    }

    // Use printerport to trigger the scope
    uint16_t base=0xec00;
    if(ioperm(base, 8, 1)) {
	perror("request_region failed\n");
	return NULL;
    }

    for(size_t i=0; i<table.size(); i++) {
	auto &x=table[i];
        ////////////////////////////////////////////////////////////////////////
	// timebase and send synchronisation
        ////////////////////////////////////////////////////////////////////////
	time++;
	struct timespec timestamp;
	if(clock_gettime(CLOCK_MONOTONIC, &x.timestamp)) {
	    perror("clock_gettime");
	    return NULL;
	}

	ts.tv_nsec+=222222;
	while(ts.tv_nsec>=1'000'000'000) {
	    ts.tv_sec++;
	    ts.tv_nsec-=1'000'000'000;
	}

	skt.send(sync_ns::send_t(skt,mac,ip,x.timestamp,ts));

        ////////////////////////////////////////////////////////////////////////
        // Test specific code
        ////////////////////////////////////////////////////////////////////////
        if(i>1) {
	    sc.set(x.Vdelta, x.Imax);
	    auto [Iset,limit,at_speed]=sc.compute(x.Vset,
		table[i-1].Vservo, table[i-1].Vrotor[1]
	    );
	    x.Iset=Iset; x.limit=limit; x.at_speed=at_speed;
	    skt.send(motion_ns::send_t(skt, mac, ip, x.Iset, x.limit, 0));
	}

        ////////////////////////////////////////////////////////////////////////
        // Wait and receive data
        ////////////////////////////////////////////////////////////////////////
	outb(0,base);
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, nullptr);
	outb(0xff,base);

	ssize_t rx_size;
	do{
	    union rx_types {
		udp_t udp;
		sync_ns::recv_t sync;
		motion_ns::recv_t motion;
		char txt[1024];
	    } buffer;
	    rx_size=recvfrom(skt.socket(),
		&buffer, sizeof(buffer), MSG_DONTWAIT, NULL, NULL);
	    if(buffer.udp.src_port==htons(sync_ns::port)
		&& rx_size==sizeof(buffer.sync)
	    ) {
		static_cast<sync_ns::to_host&>(x)=buffer.sync;
	    } else if(buffer.udp.src_port==htons(motion_ns::port)
		&& rx_size==sizeof(buffer.motion)
	    ) {
		static_cast<motion_ns::to_host&>(x)=buffer.motion;
	    }
	} while(rx_size>0);

    }
    return NULL;
}

int main(int argc, char *argv[])
{
    struct sched_param param;
    pthread_attr_t attr;
    pthread_t thread;
    int ret;

    try {
	std::ifstream setpoints(argv[1]);
	if(!setpoints) {
	    perror(argv[1]);
	    return 1;
	}

	for(int i=0; i<1s/timebase_t(1); i++) // synchronisation for 1s
	    table.emplace_back(sync_I_t{});

	std::string s;
	while(std::getline(setpoints,s)) {
	    if(!s.size() || s[0]=='#')
		continue;
	    double Vset,Imax,Vdelta;
	    auto x=sscanf(s.c_str()," %lf %lf %lf",
		&Vset, &Imax, &Vdelta
	    );
	    if(x!=3) {
		std::cerr << "Wrong line: " << s << std::endl;
		return 1;
	    }
	    table.emplace_back(sync_I_t{.Iset=std::complex<float>{0.0,0.0},
		.Vset=float(Vset), .Imax=float(Imax), .Vdelta=float(Vdelta)
	    });
	}

	for(int i=0; i<1s/timebase_t(1); i++) // lead out of 1s
	    table.emplace_back(sync_I_t{.Iset=std::complex<float>{0,0},
		.Vset=100, .Imax=0, .Vdelta=0});
    } catch(std::string err) {
	std::cout << err;
	return 1;
    }

    if(argc>=3) {
	std::array<int,6> m;
	if(sscanf(argv[2],"%x:%x:%x:%x:%x:%x",
	    &m[0],&m[1],&m[2],&m[3],&m[4],&m[5])!=6
	) {
	    std::cout << "Invalid mac address: " << argv[2] << "\n";
	    return 1;
	}
	for(int i=0; i<m.size(); i++)
	    mac[i]=m[i];
    }


    /* Lock memory */
    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
	    printf("mlockall failed: %m\n");
	    exit(-2);
    }
    /* Initialize pthread attributes (default values) */
    ret = pthread_attr_init(&attr);
    if (ret) {
	    printf("init pthread attributes failed\n");
	    goto out;
    }
    /* Set a specific stack size  */
    ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    if (ret) {
	printf("pthread setstacksize failed\n");
	goto out;
    }
    /* Set scheduler policy and priority of pthread */
    ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    if (ret) {
	    printf("pthread setschedpolicy failed\n");
	    goto out;
    }
    param.sched_priority = 80;
    ret = pthread_attr_setschedparam(&attr, &param);
    if (ret) {
	    printf("pthread setschedparam failed\n");
	    goto out;
    }
    /* Use scheduling parameters of attr */
    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
	    printf("pthread setinheritsched failed\n");
	    goto out;
    }
    /* Create a pthread with specified attributes */
    ret = pthread_create(&thread, &attr, rt_thread, NULL);
    if (ret) {
	    printf("create pthread failed\n");
	    goto out;
    }
    /* Join the thread and wait until it is done */
    ret = pthread_join(thread, NULL);
    if (ret)
	    printf("join pthread failed: %m\n");
    for(auto &x: table) {
	std::cout << x << std::endl;
    }
out:
    return ret;
}
