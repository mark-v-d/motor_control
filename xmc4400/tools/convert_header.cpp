#include <iostream>
#include <regex>

using namespace std;

map< string,map<string,string> > output_pin;

////////////////////////////////////////////////////////////////////////////////

void print_output(ostream &s, string const &n)
{
    s   << "\ntemplate <int port, int pin>\n"
	<< "class " << n << ":public output<port,pin>\n"
	<< "{\npublic:\n    " << n 
	<< "(void) { static_assert(port<0,\"Illegal pin\");}\n"
	<< "};\n";
}

void print_output(ostream &s, string const &n, string const &i, string const &p)
{
    s   << "template <>\n"
	<< "inline " << n << '<' << i << ">::" << n
	<< "(void)\n{\n    set(" << p << ");\n}\n";
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
