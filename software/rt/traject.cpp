#include <iostream>
#include "trajectory.h"

int main(int argc, char **argv)
{
    trajectory t(atof(argv[2]), atof(argv[3]));
    t.set_move(atof(argv[1]));
    float V;
    do {
	auto [x,v,a]=t.position();
	std::cout << x << " " << v << " " << a << std::endl;
	V=v;
    } while(V);

    return 0;
}
