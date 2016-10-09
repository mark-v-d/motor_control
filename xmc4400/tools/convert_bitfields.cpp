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
//  namespace	register	start      field   length
map<string, map<string, map<int,  pair<string, int> > > > field;

////////////////////////////////////////////////////////////////////////////////


// Handle lines defining output configuration
const regex output(
    "^#define ([^_]+)_([^_]+)_([^_]+)_(.*)_Msk *\\(([^)]+).*< ([A-Z_0-9]+) ([A-Z_0-9]+): ([A-Z_0-9]+)");
int output_function(smatch const &m)
{ 
    // m[1]=unit
    // m[2]=subunit
    // m[3]=register
    // m[4]=field
    // m[5]=mask
    // m[6]=real unit
    // m[7]=register name
    // m[8]=field name
    uint32_t mask=stoul(m[5].str(),0,0);
    if(!mask)
	return 1;
    int start=0;
    while(!(mask&1)) {
	start++;
	mask>>=1;
    }
    int length=0;
    while(mask&1) {
	length++;
	mask>>=1;
    }

    field[m[6]][m[7]][start]=pair<string,int>(m[8].str(),length);
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
    for(auto subunit: field) {
	cout << "namespace " << subunit.first << "_ns {\n";
	for(auto reg: subunit.second) {
	    cout << "union " << reg.first << "_t {\n    struct {\n";
	    int lsb=0;
	    for(auto start: reg.second) {
		int fill=start.first-lsb;
		if(fill<0)
		    throw logic_error("Oops");
		if(fill)
		    cout << "        uint32_t :" <<fill <<";\n";
		cout << "        uint32_t " << start.second.first
		    << ":" << start.second.second << ";\t// " 
		    << start.first << endl;
		lsb+=start.second.second+fill;
	    }
	    cout << "    };\n    uint32_t raw;\n};\n";
	}
	cout << "}\n";
    } 
    return 0;
}
