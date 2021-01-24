#
# Returns the address and size of a variable
#
function [address sz t]=location(a,name)
	[s, e, te, m, t, nm, sp]=regexp(name, "([^{}().]+)([{}().]*)");
	address=a.var.(t{1}{1}).address;
	type=a.var.(t{1}{1}).type;
	[address sz t]=recurse(address,type,t);
endfunction

function [address sz t]=recurse(address,type,t)
	switch t{1}{2}
	case "{" [address sz t]=cell_index(address,type,{t{2:end}});
	case {"}." "." ")."} [address sz t]=name_index(address,type,
		{t{2:end}});
	case {"" "}"} sz=type.size;
	end
endfunction

function [address sz t]=cell_index(address,type,t)
	index=str2num(t{1}{1});
	assert(index<=type.upper_bound)
	address+=index*type.type.size;
	[address sz t]=recurse(address,type.type,t);
endfunction

function [address sz t]=name_index(address,type,t)
	x=type.structure.(t{1}{1});
	address+=x.data_member_location;
	[address sz t]=recurse(address,x.type,t);
endfunction
