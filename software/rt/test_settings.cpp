#include "ss.h"

constexpr int order=3;
constexpr int inputs=2;
constexpr int outputs=1;

ss_t<order,inputs,outputs> controller;

int main(int argc, char **argv)
{
	std::ifstream settings(argv[1]);
	if(!settings) {
	    perror(argv[1]);
	    return 1;
	}

	controller.read(settings);
	return 0;
}
