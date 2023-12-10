#ifndef SS_H
#define SS_H

#include <iostream>
#include <fstream>
#include <string>
#include <eigen3/Eigen/Dense>

/*
#include <fmt/core.h>
#include <fmt/ranges.h>

namespace std { using fmt::format; }
*/

template <class T,int rows,int cols>
void read(Eigen::Matrix<T,rows,cols> &m, std::string s)
{
    std::vector<double> v;
    size_t start=0;
    try {
	while(s.size()) {
		v.push_back(stod(s,&start));
	    s.erase(0,start);
	}
    } catch(std::invalid_argument& ia) {
	;
    }
    if(v.size()!=m.reshaped().size()) {
	throw(std::format("Wrong matrix size, is {}, should be {}",
	    v.size(), m.reshaped().size()));
    }

    for(auto &x: m.reshaped()) {
	x=v.front();
	v.erase(v.begin());
    }
}

template <size_t order, size_t inputs, size_t outputs>
class ss_t {
    Eigen::Matrix<double,order,order> A;
    Eigen::Matrix<double,order,inputs> B;
    Eigen::Matrix<double,outputs,order> C;
    Eigen::Matrix<double,outputs,inputs> D;
    Eigen::Matrix<double,order,outputs> K;

    Eigen::Matrix<double,outputs,1> limit;
    Eigen::Matrix<double,order,1> state;
public:
    using input_t=Eigen::Matrix<double,inputs,1>;
    using output_t=Eigen::Matrix<double,outputs,1>;

    int read(std::istream &in) {
	std::string s;
	int line=0;
	while(std::getline(in,s)) {
	    if(!s.size() || s[0]=='#')
		continue;
	    try {
		switch(line++) {
		case 0: ::read(A,s); break;
		case 1: ::read(B,s); break;
		case 2: ::read(C,s); break;
		case 3: ::read(D,s); break;
		case 4: ::read(K,s); break;
		case 5: ::read(limit,s); return 0;
		}
	    } catch (std::string err) {
		std::cerr << std::format("ERROR: {}, @line {} = {}\n",
		    err, line, s);
		return 1;
	    }
	}
	throw(std::format("ERROR: {} lines were converted, {} are required\n",
	    line, 6));
    }


    output_t compute(input_t const &input) {
	output_t output=C*state+D*input;
	state=A*state+B*input;
	auto limited=output;
	for(int i=0; i<outputs; i++) {
	    if(output(i,0)>limit(i,0))
		limited(i,0)=limit(i,0);
	    else if(output(i,0)<-limit(i,0))
		limited(i,0)=-limit(i,0);
	}
	state+=K*(limited-output);

	return limited;
    }
};

#endif
