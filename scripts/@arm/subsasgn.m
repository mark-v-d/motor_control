function a=subsasgn(a,s,val)
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
	data=[0:length(data)-1;data];
	do_and_wait(a,sprintf("array unset x;array set x {%s}",
		sprintf("%d ",data(:))));
	do_and_wait(a,sprintf("array2mem x 8 %d %d", address, size(data,2)));
endfunction

function result=makedata(value, info)
	switch(info.decode)
	case "enum"
		tt=sprintf("uint%d",8*info.type.size);
		if(isnumeric(value))
			result=cast(value,tt);
		else
			result=cast(info.type.enum.(value),tt);
		end
	otherwise
		result=cast(value,info.decode);
	end
	result=typecast(result,"uint8");
endfunction

function newdot(a,s,val,info,address=0)
	if !length(s)
		assert(!isfield(info.type,"upper_bound")  ||
			length(val)==info.type.upper_bound+1,
				"Vector assignment lengths must match");
		write(a,address,makedata(val,info));
		return
	end

	switch(s(1).type)
	case "."
		assert(s(1).type==".", "structure needs to be indexed with .");
		assert(info.decode=="structure", "bla");
		address+=info.type.structure.(s(1).subs).data_member_location;
		newdot(a,s(2:end),val,
			info.type.structure.(s(1).subs), address);
	case "()"
		assert(length(s.subs)==1,"Only 1D arrays are supported");
		assert(length(s.subs{1})==length(val), "lengths must match");
		assert(max(s.subs{1})<info.type.upper_bound+2,
			"Outside valid range");
		for x=1:length(s.subs{1})
			write(a,address+(s.subs{1}(x)-1)*info.type.size,
				makedata(val(x), info));
		end
	otherwise
	end
endfunction
