function len=block_size(a, v)
	if isfield(v.type,"upper_bound")
		len=v.type.type.size;
		len*=v.type.upper_bound+1;
	else
		len=v.type.size;
	end
endfunction
