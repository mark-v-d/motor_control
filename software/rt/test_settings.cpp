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

    std::string s;
    while(std::getline(std::cin,s)) {
	if(!s.size() || s[0]=='#')
	    continue;
	decltype(controller)::input_t input;
	read(input,s);
	auto output=controller.compute(input);
	std::cout << output << std::endl;
    }
}
