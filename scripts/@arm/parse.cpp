#include <map>
#include <iostream>
#include <sstream>
#include <set>
#include <regex>

#include "elf++.hh"
#include "dwarf++.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <cxxabi.h>

class type_t;
class member_t;

using namespace std::string_literals;

std::shared_ptr<type_t> create_type(const dwarf::die &node);

std::string demangle(std::string mangled_name)
{
    int status = 0;
    const char *realname =
	abi::__cxa_demangle(mangled_name.c_str(), 0, 0, &status);
    switch (status) {
    case 0:{
	    std::string result { realname};
	    free((void *) realname);
	    return result;
	}
	break;
    case -1:
	throw std::string("FAIL: failed to allocate memory while demangling");
    case -2:
	return mangled_name;
    default:
	throw std::string("FAIL: unexpected error for __cxa_demangle");
    }
}

std::map<std::string,std::tuple<int64_t,int64_t>> variables;
std::map<std::string,std::shared_ptr<member_t>> globals;
std::map<std::string,std::vector<std::string>> unfinished_variable;
std::set<std::string> completed;
////////////////////////////////////////////////////////////////////////////////

void dump_attributes(const dwarf::die &node, int depth = 0)
{
    for (auto &attr : node.attributes()) {
	printf("# %*.s      %s %s\n", depth, "",
	       to_string(attr.first).c_str(),
	       to_string(attr.second).c_str());
    }
}


////////////////////////////////////////////////////////////////////////////////
/* All data object root(s) */
class type_t {
public:
    type_t(const dwarf::die &node) {}
    virtual void dump(std::ostream&,int)=0;
protected:
    type_t(void) {}
};

class type_name_t {
protected:
    std::string name;
public:
    type_name_t(const dwarf::die &node) {
	if(!node.has(dwarf::DW_AT::name))
	    throw("Missing name"s);
	name=node.resolve(dwarf::DW_AT::name).as_string();
    }
    std::string const &get_name(void) { return name; }
protected:
    type_name_t(void) {}
};

class missing_type_size_t {};

class type_size_t {
protected:
    uint64_t size;
public:
    type_size_t(const dwarf::die &node)
    {
	if(!node.has(dwarf::DW_AT::byte_size))
	    throw(missing_type_size_t{});
	    //throw(std::string("Missing DW_AT::byte_size"));
	size=node.resolve(dwarf::DW_AT::byte_size).as_uconstant();
    }
protected:
    type_size_t(void) {}
};

////////////////////////////////////////////////////////////////////////////////
class base_type_t:public type_t,public type_size_t, public type_name_t {
    enum encoding_t {
	BOOL=2,
	COMPLEX=3,
	FLOAT=4,
	INT=5,
	UINT=7,
	CHAR=8
    } encoding;
public:
    base_type_t(const dwarf::die &node);
    void dump(std::ostream &o,int indent) {
	using std::to_string;
	std::string d;
	switch(encoding) {
	case BOOL:
	case INT:
	    d="int";
	    d+=to_string(size*8);
	    break;
	case UINT:
	    d="uint";
	    d+=to_string(size*8);
	    break;
	case CHAR:
	    if(size==1)
		d="char";
	    else {
		d="int";
		d+=to_string(size*8);
	    }
	    break;
	case FLOAT:
	    if(size==4)
		d="single";
	    else if(size==8)
		d="double";
	    else
		throw(std::string("Unexpected floating point size"));
	    break;
	case COMPLEX:
	    if(size==8)
		d="single complex";
	    else if(size==16)
		d="double complex";
	    else
		throw(std::string("Unexpected complex size"));
	    break;
	}

	o << "struct(\"size\", " << size
	    << ", \"proc\", @base_type, \"decode\", \"" << d << "\")";
    }
};

base_type_t::base_type_t(const dwarf::die &node):
    type_t(node),
    type_size_t(node),
    type_name_t(node)
{
    encoding=static_cast<encoding_t>(
	node.resolve(dwarf::DW_AT::encoding).as_uconstant());
}

////////////////////////////////////////////////////////////////////////////////
class inheritance_t:public type_t {
protected:
    int64_t location;
    std::shared_ptr<type_t> data;
    bool external;
public:
    inheritance_t(const dwarf::die &node): external(false) {
	if(node.has(dwarf::DW_AT::data_member_location)) {
	    auto attr=node.resolve(dwarf::DW_AT::data_member_location);
	    if(attr.get_type()==dwarf::value::type::exprloc) {
		auto x=attr.as_exprloc();
		dwarf::expr_context ctx;
		auto y=x.evaluate(&ctx, {0});
		location=y.value;
	    } else
		location=attr.as_uconstant();
	} else if(node.has(dwarf::DW_AT::external) &&
	    node.resolve(dwarf::DW_AT::external).as_flag()
	) {
	    external=1;
	    location=-1;
	} else
	    throw(to_string(node.tag)+" no offset and not external");
	data=create_type(node.resolve(dwarf::DW_AT::type).as_reference());
    }

    void dump(std::ostream &o,int indent);
};

////////////////////////////////////////////////////////////////////////////////
class member_t:public inheritance_t, public type_name_t {
public:
    member_t(const dwarf::die &node):inheritance_t(node), type_name_t(node) {}
    void dump(std::ostream &o,int indent) {
	if(location<0)
	    return;
	if(!external) {
	    std::string i(indent,' ');
	    o << i << "\"" << name << "\", struct(\"data_member_location\", "
		<< location << ", \"type\", ";
	    data->dump(o,indent+4);
	    o << ")";
	} else {
	    // o << "# external: " << demangle(get_name()) << std::endl;
	    data->dump(o,indent+4);
	}
    }
    void dump_single(std::ostream &o,int indent) {
	if(location<0)
	    return;
	std::string i(indent,' ');
	data->dump(o,indent);
    }
    bool is_external(void) { return external; }
    void set_location(int64_t loc) { location=loc; }
    int64_t get_location(void) { return location; }
};


////////////////////////////////////////////////////////////////////////////////
class array_t:public type_t {
    std::shared_ptr<type_t> type;
    int64_t upper_bound;
public:
    array_t(const dwarf::die &node);
    void dump(std::ostream &o,int indent) {
	std::string i(indent,' ');
	o << "struct(\"upper_bound\", " << upper_bound
	    << ", \"proc\", @array_t, \"type\", ";
	// o << i << "array: " << upper_bound << std::endl;
	type->dump(o,indent+4);
	o << ")";
    }
};

array_t::array_t(const dwarf::die &node):type_t(node) {
    using dwarf::DW_TAG;
    using dwarf::DW_AT;

    for (auto &child: node) {
	std::cout << "# array child " << to_string(child.tag) << std::endl;
	switch(child.tag) {
	case DW_TAG::subrange_type:
	    if(child.has(DW_AT::upper_bound))
		upper_bound=child.resolve(DW_AT::upper_bound).as_uconstant();
	    else
		upper_bound=-1;
	    break;
	default:
	    throw(to_string(child.tag)+" unhandled child of array_type");
	}
    }
    type=create_type(node.resolve(dwarf::DW_AT::type).as_reference());
}


////////////////////////////////////////////////////////////////////////////////
class class_t:public type_t,public type_size_t {
    std::vector<std::shared_ptr<type_t>> parent;
    std::vector<std::shared_ptr<member_t>> member;
    std::string name;
public:
    class_t(const dwarf::die &node);
    void dump(std::ostream &o,int indent) {
	std::string i(indent,' ');
	if(parent.size()==0 && member.size()==1) {
	    // o << i << "Class single member\n";
	    member[0]->dump_single(o,indent);
	} else {
	    o << "struct(\"size\", " << size
		<< ", \"proc\", @class_t, \"structure\", struct(\n";
	    int i=0;
	    for(auto p: parent) {
		if(i++)
		    o << ",\n";
		p->dump(o,indent+4);
	    }
	    for(auto m: member) {
		if(m->is_external())
		    continue;
		if(i++)
		    o << ",\n";
		m->dump(o,indent+4);
	    }
	    if(!i)
		o << "\"dummy\", 0";
	    o << "\n))";
	}
    }

    std::string const &get_name(void) { return name; }
};

class_t::class_t(const dwarf::die &node):type_t(node),type_size_t(node)
{
    using dwarf::DW_TAG;

    if(node.has(dwarf::DW_AT::name))
	name=node.resolve(dwarf::DW_AT::name).as_string();

    for (auto &child : node) {
	switch(child.tag) {
	case DW_TAG::member: {
	    auto m=std::dynamic_pointer_cast<member_t>(create_type(child));
	    if(m->is_external()) {
		std::string mnam=get_name()+"::"+m->get_name();
		if(variables.count(mnam)) {
		    std::cout << "# Address found "<<std::get<0>(variables[mnam]);
		    m->set_location(std::get<0>(variables[mnam]));
		    globals[mnam]=m;
		} else {
		    std::cout << "# Forgetting " << m->get_name()
			<< " part of " << this->get_name()
			<< " mnam=" << mnam
			<< std::endl;
		}
	    } else
		member.push_back(m);
	    } break;
	case DW_TAG::imported_declaration: break;
	//case DW_TAG::GNU_template_parameter_pack: break;
	case DW_TAG::inheritance: parent.push_back(create_type(child)); break;
	case DW_TAG::subprogram: break;
	case DW_TAG::template_value_parameter: break;
	case DW_TAG::typedef_: break;
	case DW_TAG::const_type: break;
	case DW_TAG::template_type_parameter: break;
	case DW_TAG::union_type: break;
	case DW_TAG::volatile_type: break;
	case DW_TAG::enumeration_type: break;
	case DW_TAG::class_type: break;
	default:
	    std::cout << "# FAIL " << to_string(child.tag) << std::endl;
	    dump_attributes(child);
	    throw(to_string(child.tag)+" unexpected child of class");
	}
    }
}

void inheritance_t::dump(std::ostream &o,int indent) {
    auto p=std::dynamic_pointer_cast<class_t>(data);
    std::string i(indent,' ');
    if(!p)
	throw("Inheritance not from class?");
    else {
	o << i << "\"" << p->get_name()
	    << "\", struct(\"data_member_location\", " << location
	    << ", \"type\", ";
	data->dump(o,indent+4);
	o << ')';
    }
}


////////////////////////////////////////////////////////////////////////////////
class union_t:public type_t,public type_size_t {
    std::vector<std::shared_ptr<type_t>> parent;
    std::vector<std::shared_ptr<member_t>> member;
public:
    union_t(const dwarf::die &node);
    void dump(std::ostream &o,int indent) {
	std::string i(indent,' ');
	if(parent.size()==0 && member.size()==1) {
	    // o << i << "Class single member\n";
	    member[0]->dump_single(o,indent);
	} else {
	    o << "struct(\"size\", " << size
		<< ", \"proc\", @union_t, \"structure\", struct(\n";
	    int i=0;
	    for(auto p: parent) {
		if(i++)
		    o << ",\n";
		p->dump(o,indent+4);
	    }
	    /*
	    for(auto m: member) {
		if(m->is_external())
		    continue;
		if(i++)
		    o << ",\n";
		m->dump(o,indent+4);
	    }
	    */
	    if(!i)
		o << "\"dummy\", 0";
	    o << "\n))";
	}
    }
};

union_t::union_t(const dwarf::die &node):type_t(node),type_size_t(node)
{
    using dwarf::DW_TAG;

    for (auto &child : node) {
	switch(child.tag) {
	case DW_TAG::member:
	    break;
	case DW_TAG::inheritance: parent.push_back(create_type(child)); break;
	case DW_TAG::subprogram: break;
	case DW_TAG::template_value_parameter: break;
	case DW_TAG::typedef_: break;
	case DW_TAG::const_type: break;
	case DW_TAG::template_type_parameter: break;
	default:
	    throw(to_string(child.tag)+" unexpected child of union");
	}
    }
}

////////////////////////////////////////////////////////////////////////////////
class enumeration_t:public type_t,public type_size_t {
    std::map<std::string,int> value;
public:
    enumeration_t(const dwarf::die &node);
    void dump(std::ostream &o,int indent) {
	std::string i(indent,' ');
	o << "struct(\"size\", " << size
	    << ", \"proc\", @enum_t, \"enum\", struct(\n";
	int c=0;
	for(auto m: value) {
	    if(c++)
		o << ",\n";
	    o << " \"" << m.first << "\", " << m.second;
	}
	o << "\n),\n \"decode\", \"enum\")";
    }
};

enumeration_t::enumeration_t(const dwarf::die &node):type_t(node),type_size_t(node)
{
    using dwarf::DW_TAG;

    for (auto &child : node) {
	switch(child.tag) {
	case DW_TAG::enumerator: {
	    auto name=child.resolve(dwarf::DW_AT::name).as_string();
	    int v;
	    try {
		v=child.resolve(dwarf::DW_AT::const_value).as_uconstant();
	    } catch(dwarf::value_type_mismatch &)  {
		v=child.resolve(dwarf::DW_AT::const_value).as_sconstant();
	    }
	    value[name]=v;
	    } break;
	default:
	    throw(to_string(child.tag)+" unexpected child of enumeration");
	}
    }
}

////////////////////////////////////////////////////////////////////////////////
class pointer_t:public type_t {
public:
    pointer_t(const dwarf::die &node):type_t(node)
    {
    }
    void dump(std::ostream &o,int indent) {
	o << std::string(indent,' ')
	    << "struct(\"size\", 4, \"proc\", @base_type, \"decode\", \"uint32\")";
    }
};


std::shared_ptr<type_t> create_type(const dwarf::die &node)
{
    static int level=0;
    class t {
    public:
	t(void) {}
	~t(void) { std::cout << "# CREATE " << --level << " DONE\n"; }
    } A;
    using dwarf::DW_TAG;
    std::cout << "# CREATE " << level++ << " TYPE " << to_string(node.tag) << std::endl;
    dump_attributes(node,2);

    switch(node.tag) {
    case DW_TAG::pointer_type:   return std::make_shared<pointer_t>(node);
    case DW_TAG::base_type: 	 return std::make_shared<base_type_t>(node);
    case DW_TAG::array_type:     return std::make_shared<array_t>(node);

    case DW_TAG::structure_type:
    case DW_TAG::class_type:     return std::make_shared<class_t>(node);

    case DW_TAG::union_type:     return std::make_shared<union_t>(node);

    case DW_TAG::enumeration_type:return std::make_shared<enumeration_t>(node);

    case DW_TAG::member: 	 return std::make_shared<member_t>(node);
    case DW_TAG::inheritance: 	 return std::make_shared<inheritance_t>(node);

    case DW_TAG::reference_type:
    case DW_TAG::volatile_type:
    case DW_TAG::typedef_:
    case DW_TAG::const_type:
	return create_type(node.resolve(dwarf::DW_AT::type).as_reference());


    default:
	throw(to_string(node.tag) + " unhandled type");
    }
}

void dump_variables(const dwarf::die &node, int depth = 0, std::string ns={})
{
    std::stringstream buffer;
    if(node.tag==dwarf::DW_TAG::variable
	&& node.has(dwarf::DW_AT::name)
	&& node.has(dwarf::DW_AT::type)
	&& (variables.count(at_name(node))
	    || variables.count(ns +"::"+at_name(node)
	)
	)
    ) {
	printf("# %*.s<%" PRIx64 "> %s\n", depth, "",
	       node.get_section_offset(),
	       to_string(node.tag).c_str());
	dump_attributes(node,depth);

	auto name=node.resolve(dwarf::DW_AT::name).as_string();
	if(ns.size())
	    name=ns + "::" + name;
	auto address=std::get<0>(variables[name]);
	//variables.erase(name);
	if(ns.size())
	    name="(\""s +  name + "\")";
	if(!completed.count(name)) {
	std::cout << "var." << name << ".address=0x" << std::hex << address
	    << std::dec << ";\n";
	try {
	    auto type=node.resolve(dwarf::DW_AT::type).as_reference();
	    auto result=create_type(type);
	    buffer << "var." << name << ".type=";
	    result->dump(buffer,0);
	    buffer << ";\n";
	    variables.erase(name);
	    completed.insert(name);
	} catch(missing_type_size_t) {
	    buffer << "# Postponing until type defined\n";
	    auto type=node.resolve(dwarf::DW_AT::type).as_reference();
	    std::string type_name=type.resolve(dwarf::DW_AT::name).as_string();
	    unfinished_variable[type_name].push_back(name);
	} catch(std::string &s) {
	    buffer << "# FAILED with: " << s << std::endl;
	} catch(dwarf::format_error &s) {
	    buffer << "# format error\n";
	}
	std::cout << buffer.rdbuf();
	}
    } else if(
	node.tag==dwarf::DW_TAG::class_type
	&& node.has(dwarf::DW_AT::name)
    ) {
	auto type_name=node.resolve(dwarf::DW_AT::name).as_string();
	std::cout << "# TYPE " << type_name << std::endl;
	auto vars=unfinished_variable.find(type_name);
	if(vars!=unfinished_variable.end()) {
	    try {
		auto result=create_type(node);
		for(auto name: vars->second) {
		    if(completed.count(name))
			continue;
		    buffer << "var." << name << ".type=";
		    result->dump(buffer,0);
		    buffer << ";\n";
		    completed.insert(name);
		}
		std::cout << buffer.rdbuf();
	    } catch(missing_type_size_t) {
		std::cout << "# Missing size\n";
	    }
	}
    } else if(
	node.tag==dwarf::DW_TAG::variable
	&& node.has(dwarf::DW_AT::name)
    ) {
	auto name=node.resolve(dwarf::DW_AT::name).as_string();
	std::cout << "# Skipping: " << name << std::endl;
	/*
	std::cout << "# variables.count="
	    << variables.count(at_name(node)) << " "
	    << variables.count(ns +"::"+at_name(node))
	    << std::endl;
	dump_attributes(node,depth);
	*/
    } else if(node.tag==dwarf::DW_TAG::namespace_
	&& node.has(dwarf::DW_AT::name)
    ) {
	ns+=node.resolve(dwarf::DW_AT::name).as_string();
    }
    for (auto &child : node)
	dump_variables(child, depth + 1,ns);
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
	    if(d.type()==elf::stt::func)
		continue;
	    variables[demangle(sym.get_name())]={d.value,d.size};
	}
    }
    /*
    for(auto x: variables) {
	std::cout << "[" << x.first << "]=" <<
	    std::get<0>(x.second) << " , " << std::get<1>(x.second) << std::endl;
    }
    */

    try {
	for (auto cu : dw.compilation_units()) {
	    auto d=cu.root();
	    if(d.has(dwarf::DW_AT::name))
		std::cout << "# Compile unit: "
		    << d.resolve(dwarf::DW_AT::name).as_string()
		    << std::endl;
	    else
		std::cout <<"# Unnamed compile unit\n";
	    dump_variables(d);
	}
    } catch(std::string &s) {
	std::cerr << "failed with: " << s << std::endl;
    }

    for(auto g: globals) {
	auto name=std::regex_replace(g.first,std::regex("::"),"__");
	std::cout << "# GLOBAL " << name << std::endl;
	std::cout << "var." << name << ".address=0x" << std::hex
	    << g.second->get_location()
	    << std::dec << ";\n";

	std::stringstream buffer;
	try {
	    buffer << "var." << name << ".type=";
	    g.second->dump(buffer,0);
	    buffer << ";\n";
	} catch(std::string &s) {
	    buffer << "# FAILED with: " << s << std::endl;
	}
	std::cout << buffer.rdbuf();
    }
}
