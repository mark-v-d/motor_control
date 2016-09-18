#include "errno.h"

void unimplemented(void) { for(;;); }
void _exit(int i) { unimplemented(); for(;;); }
void _kill() { unimplemented(); }
int _getpid() { unimplemented(); }
void *_sbrk(ptrdiff_t size) 
{ 
    extern char Heap_Bank1_Start;
    extern char Heap_Bank1_End;
    static char *p=&Heap_Bank1_Start;
    if(p+size<&Heap_Bank1_End) {
	p+=size;
	return p;
    } else {
	errno=-ENOMEM;
	return (void*)-1;
    }
}
void _read() { unimplemented(); }
void _close() { unimplemented(); }
void _fstat() { unimplemented(); }
void _lseek() { unimplemented(); }
void _isatty() { unimplemented(); }
void _write() { unimplemented(); }
void _open() { unimplemented(); }
