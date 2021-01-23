#ifndef ATOMIC_WRAPPING_COUNTER_H
#define ATOMIC_WRAPPING_COUNTER_H

#include <atomic>

template<typename T,int max>
class atomic_wrapping_counter {
    std::atomic<T> counter;
public:
    atomic_wrapping_counter(T o):counter(o) {}
    T operator++(int) {
	T t,n;
	t=counter;
	do {
	    n=t+1;
	    if(n>=max)
		n=0;
	} while(!counter.compare_exchange_strong(t,n));
	return t;
    }

    T operator++(void) {
	T t,n;
	t=counter;
	do {
	    n=t+1;
	    if(n>=max)
		n=0;
	} while(!counter.compare_exchange_strong(t,n));
	return n;
    }
};
#endif
