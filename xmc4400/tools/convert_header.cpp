#include <iostream>
#include <regex>
#include <cstdlib>

using namespace std;

map< string,map<string,string> > output_pin;

////////////////////////////////////////////////////////////////////////////////

void print_output(ostream &s, string const &n)
{
    s   << "\ntemplate <int port, int pin>\n"
	<< "class " << n << ":public output<port,pin>\n"
	<< "{\npublic:\n    " << n 
	<< "(void) { static_assert(port<0,\"Illegal pin\");}\n";
    smatch m;
    // CCU8 conversions to interface with XMCLib
    if(regex_search(n, m, regex("CCU8([0-3])_OUT([0-3])([0-3])"))) {
	s   << "    operator XMC_CCU8_MODULE_t*() { return CCU8" << m[1] << "; }\n"
	    << "    operator XMC_CCU8_SLICE_t*() { return CCU8" << m[1] << "_CC8" << m[2] << ";}\n"
	    << "    operator uint8_t() { return " << m[2] << "; }\n"
	    << "    void operator=(uint32_t i) { CCU80_CC8" << m[2] << "->CR" << atoi(m[3].str().c_str())/2+1 << "S=i; }\n";
    }
    s << "};\n";
}

void print_output(ostream &s, string const &n, string const &i, string const &p)
{
    s   << "template <>\n"
	<< "inline " << n << '<' << i << ">::" << n;
    if(regex_search(p, regex("HWCTRL")))
	s << "(void)\n{\n    set(" << p << ");\n}\n";
    else
	s << "(void)\n{\n    set(XMC_GPIO_MODE_t(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | " << p << "));\n}\n";
}

void flush_output_pin(void)
{
	for(auto p: output_pin) {
	    print_output(cout,p.first);
	    for(auto pp: p.second)
		print_output(cout,p.first,pp.first,pp.second);
	}
	output_pin.clear();
	cout << "#endif\n";
}

// Handle device_package ifdefs
const regex device("^(#if.*UC_DEVICE.*UC_PACKAGE.*)");
int device_function(smatch const &m)
{
    static bool conditional=false;
    if(conditional)
	flush_output_pin();
    conditional=true;
    cout << m[0];
    return 1;
}

// Handle lines defining output configuration
const regex output(
    "^#define P([0-9]+)_([0-9]+)_[^_]+_([^ \t]+)[ \t]*([^[:space:]]+)");
int output_function(smatch const &m)
{ 
    output_pin[m[3]][m[1].str()+","+m[2].str()]=m[4];
    return 1;
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    struct awk {
	regex exp;
	int (*func)(smatch const&);
    };
    vector<awk> expr={
	{device, device_function},
	{output, output_function}
    };
    string line;

    while(getline(cin,line)) {
	for(auto e: expr) {
	    smatch m;
	    if(regex_search(line, m, e.exp))
		if((e.func)(m))
		    break;
	}
    }
    flush_output_pin();
    return 0;
}
