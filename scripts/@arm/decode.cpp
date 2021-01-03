#include <map>
#include <iostream>
#include <set>

#include "elf++.hh"
#include "dwarf++.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>

struct location {
    uint64_t address;
    uint64_t size;
};

std::map<std::string,location> variables;
std::set<int> used_offset;

class use_offset_t {
    int use;
public:
    use_offset_t(const dwarf::die &node);
    ~use_offset_t(void);
    operator bool(void) { return use; }
};

use_offset_t::use_offset_t(const dwarf::die &node)
{
    if(!used_offset.count(node.get_section_offset())) {
	used_offset.insert(node.get_section_offset());
	use=node.get_section_offset();
    } else
	use=0;
}

use_offset_t::~use_offset_t(void)
{
    if(use)
	used_offset.erase(use);
}

void dump_type(const dwarf::die &node, int depth = 0)
{
    use_offset_t use(node);
    if(!use) {
	std::cout << "BREAKING OFFSET LOOP " << std::hex <<
	    node.get_section_offset() << std::endl;
	return;
    }
    printf("%*.s<%" PRIx64 "> %s\n", depth, "",
	   node.get_section_offset(),
	   to_string(node.tag).c_str());

    switch(node.tag) {
    case dwarf::DW_TAG::pointer_type:
	std::cout << "pointer\n";
	return;
    case dwarf::DW_TAG::structure_type:
	break;
    case dwarf::DW_TAG::volatile_type:
    case dwarf::DW_TAG::typedef_:
    case dwarf::DW_TAG::const_type:

	break;
    }
    for (auto &attr : node.attributes()) {
	printf("%*.s      %s %s\n", depth, "",
	       to_string(attr.first).c_str(),
	       to_string(attr.second).c_str());
	if(attr.first!=dwarf::DW_AT::type)
	    continue;
    }
    if(node.has(dwarf::DW_AT::type)) {
	auto type=node.resolve(dwarf::DW_AT::type).as_reference();
	dump_type(type,depth+1);
    }
    for (auto &child : node) {
	if(child.tag==dwarf::DW_TAG::subprogram
	    || child.tag==dwarf::DW_TAG::formal_parameter
	)
	    continue;
	dump_type(child, depth + 1);
    }
}

void dump_variables(const dwarf::die &node, int depth = 0)
{
    use_offset_t use(node);
    if(!use) {
	std::cout << "BREAKING OFFSET LOOP " << std::hex <<
	    node.get_section_offset() << std::endl;
	return;
    }


    if(node.tag==dwarf::DW_TAG::variable  && node.has(dwarf::DW_AT::name) ) {
	printf("%*.s<%" PRIx64 "> %s\n", depth, "",
	       node.get_section_offset(),
	       to_string(node.tag).c_str());
	for (auto &attr : node.attributes()) {
	    printf("%*.s      %s %s\n", depth, "",
		   to_string(attr.first).c_str(),
		   to_string(attr.second).c_str());
	}

    }

    if(node.tag==dwarf::DW_TAG::variable
	&& node.has(dwarf::DW_AT::name)
	&& node.has(dwarf::DW_AT::location)
	// && node.resolve(dwarf::DW_AT::location).get_type()==dwarf::value::type::exprloc
	&& variables.count(at_name(node))
    ) {
	auto x=node.resolve(dwarf::DW_AT::location).as_exprloc();
	printf("%*.s<%" PRIx64 "> %s\n", depth, "",
	       node.get_section_offset(),
	       to_string(node.tag).c_str());
	for (auto &attr : node.attributes()) {
	    printf("%*.s      %s %s\n", depth, "",
		   to_string(attr.first).c_str(),
		   to_string(attr.second).c_str());
	}
	auto e=node.resolve(dwarf::DW_AT::location).as_exprloc().evaluate(NULL);
	std::cout << std::string(depth,' ')
	    << "LOCATION: "
	    << std::hex << "0x" << e.value
	    << std::endl;
	if(node.has(dwarf::DW_AT::type)) {
	    std::cout << "Has type----------------------------------------------------------------------\n";
	    auto type=node.resolve(dwarf::DW_AT::type).as_reference();
	    dump_type(type,depth+1);
	    std::cout << "------------------------------------------------------------------------------\n";
	}
    }
    for (auto &child : node)
	dump_variables(child, depth + 1);
}


int main(int argc, char **argv)
{
    int fd = open(argv[1], O_RDONLY);
    elf::elf f(elf::create_mmap_loader(fd));
    dwarf::dwarf dw(dwarf::elf::create_loader(f));

    for (auto &sec : f.sections()) {
	if( sec.get_hdr().type != elf::sht::symtab &&
	    sec.get_hdr().type != elf::sht::dynsym
	)
	    continue;
	int i = 0;
	for (auto sym : sec.as_symtab()) {
	    auto &d = sym.get_data();
	    if(d.type()!=elf::stt::object || d.binding()!=elf::stb::global)
		continue;
	    variables[sym.get_name()]={d.value,d.size};
	}
    }

    for (auto cu : dw.compilation_units()) {
	dump_variables(cu.root());
    }
}
