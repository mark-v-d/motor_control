#include <iostream>
#include <regex>
#include <cstdlib>

using namespace std;

/*
    output_pin[function][instantiation]=method
    where:
	function is something like "U1C0_DOUT0"
	instantiation is the port pin pair "1,5" 
	method is "XMC_GPIO_MODE_OUTPUT_ALT2"
*/

map< string,map<string,string> > output_pin;
string device_name;

////////////////////////////////////////////////////////////////////////////////

void print_output(ostream &s, string const &n)
{
    s	<< "// " << device_name;
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
	    << "    enum { module=" <<m[1] << ", slice=" << m[2] << "};\n"
	    << "    void operator=(uint32_t i) { CCU80_CC8" << m[2] << "->CR" << atoi(m[3].str().c_str())/2+1 << "S=i; }\n";
    }
    s << "};\n";
}

void print_output(ostream &s, string const &name, string const &instance, string const &method)
{
    s   << "template <>\n"
	<< "inline " << name << '<' << instance << ">::" << name;
    if(regex_search(method, regex("HWCTRL")))
	s << "(void)\n{\n    set(" << method << ");\n}\n";
    else
	s << "(void)\n{\n    set(XMC_GPIO_MODE_t(XMC_GPIO_MODE_OUTPUT_PUSH_PULL | " << method << "));\n"
	"    set(XMC_GPIO_HWCTRL_DISABLED);\n}\n";
}

void flush_output_pin(void)
{
	for(auto function: output_pin) {
	    print_output(cout,function.first);
	    for(auto method: function.second)
		print_output(cout,function.first,method.first,method.second);
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
    device_name=m[0];
    return 1;
}

// Translate P._._HWCTRL_U.C0_DOUT. to SSC.
const regex usic_dout(
    "^#define P([0-9]+)_([0-9]+)_HWCTRL_U(.)C0_DOUT(.)[ \t]*([^[:space:]]+)");
int usic_dout_function(smatch const &m)
{ 
    // m[1]=port
    // m[2]=pin
    // m[3]=usic
    // m[4]=channel
    // m[5]=method
    string function=string("U")+m[3].str()+"C0_SSC"+m[4].str();
    output_pin[function][m[1].str()+","+m[2].str()]=m[5];
    return 1;
}

// Handle lines defining output configuration
const regex output(
    "^#define P([0-9]+)_([0-9]+)_[^_]+_([^ \t]+)[ \t]*([^[:space:]]+)");
int output_function(smatch const &m)
{ 
    // m[1]=port
    // m[2]=pin
    // m[3]=function
    // m[4]=method
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
	{usic_dout, usic_dout_function},
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
