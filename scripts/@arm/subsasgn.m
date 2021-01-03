function a=subsasgn(a,s,val)
	if 0
	nargin
	for x=1:length(s)
		type=s(x).type
		subs=s(x).subs
	end
	end
	if(isempty(s))
		error("No index");
	end
	result=[];

	switch(s(1).type)
	case "()"
		error("Not done yet");
	case "{}"
		error("sw cannot be indexed with ()");
	case "."
		if strcmp(s(1).subs,"local") || strcmp(s(1).subs,"var")
			error("Cannot alter value of local or var");
		else
			newdot(a,s(2:end),val,
				a.var.(s(1).subs),
				a.var.(s(1).subs).address);
		end
	otherwise
		error("This is unexpected");
	end
endfunction

function write(a,address,data)
	if length(data)==4
		do_and_wait(a,sprintf("mww 0x%08x 0x%08x",address,
			typecast(data,"uint32")));
	else
		data=[0:length(data)-1;data];
		do_and_wait(a,sprintf("array unset x;array set x {%s}",
			sprintf("%d ",data(:))));
		do_and_wait(a,sprintf("array2mem x 8 %d %d", address,
			size(data,2)));
	end
endfunction

function result=makedata(value, info)
	switch(info.type.decode)
	case "enum"
		tt=sprintf("uint%d",8*info.type.size);
		if(isnumeric(value))
			result=cast(value,tt);
		else
			result=cast(info.type.enum.(value),tt);
		end
	case "single complex"
		result=[real(value(:))';imag(value(:))'](:)';
		result=cast(result,"single");
	otherwise
		result=cast(value,info.type.decode);
	end
	result=typecast(result,"uint8");
endfunction

function newdot(a,s,val,info,address=0)
	if !length(s)
		if(isfield(info.type,"upper_bound"))
			assert(length(val)==info.type.upper_bound+1,
				"Vector assignment lengths must match");
			write(a,address,makedata(val,info.type));
		else
			write(a,address,makedata(val,info));
		end
		return
	end

	switch(s(1).type)
	case "."
		assert(s(1).type==".", "structure needs to be indexed with .");
		address+=info.type.structure.(s(1).subs).data_member_location;
		newdot(a,s(2:end),val,
			info.type.structure.(s(1).subs), address);
	case "()"
		assert(length(s.subs)==1,"Only 1D arrays are supported");
		assert(length(s.subs{1})==length(val), "lengths must match");
		assert(max(s.subs{1})<info.type.upper_bound+2,
			"Outside valid range");
		for x=1:length(s.subs{1})
			write(a,address+(s.subs{1}(x)-1)*info.type.type.size,
				makedata(val(x), info.type));
		end
	case "{}"
		assert(length(s(1).subs)==1,"Only 1D arrays are supported");
		assert(length(s(1).subs{1})==length(val), "lengths must match");
		assert(max(s(1).subs{1})<info.type.upper_bound+2,
			"Outside valid range");
		address+=(s(1).subs{1}-1)*info.type.type.size;
		newdot(a,s(2:end),val, info.type, address);
	otherwise
	end
endfunction
