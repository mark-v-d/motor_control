function b=subsref(a,s)
	if(isempty(s))
		error("No index");
	end
	switch(s(1).type)
	case "()"
		if(length(s.subs)!=1)
			error("The address is one dimensional");
		end
		ind=s.subs{1};
		x=1;
		b=[];
		headers={};
		do
			start=ind(x);
			len=find(diff(ind(x:end))!=1);
			if(!length(len))
				len=length(ind(x:end));
			else
				len=len(1);
			end
			x+=len;
			switch(mod(len,4))
			case 0 data=do_and_wait(a,sprintf("mdw %d %d\n",start,len/4));
			otherwise data=do_and_wait(a,sprintf("mdb %d %d\n",start,len));
			end
			b=[b,decode_string(data)];
		until x>length(ind);
		size(b)
		b=typecast(b,"uint8");
	case "{}"
		error("sw cannot be indexed with ()");
	case "."
		if strcmp(s(1).subs,"local") || strcmp(s(1).subs,"var")
			b=a.(s(1).subs);
		elseif strcmp(s(1).subs,"tpi")  || strcmp(s(1).subs,"itm")
			# We don't have to read itm
			b=[];
		else
			b=dodot(a,a.var.(s(1).subs));
		end
	end
	if(length(s)>1)
		b=subsref(b,s(2:end));
	end
endfunction

function b=decode_string(td)
	lines=strsplit(char(td),"\n");
	b=[];
	for x=lines
		line=char(x{1});
		if length(line)<13 || (line(11)!=':' && line(12)!=':')
			continue;
		end
		b=[b;uint32(sscanf(line(13:end),"%x"))];
	end
	b=typecast(b,"uint8");
endfunction

function b=dodot(a,v)
	if isfield(v.type,"upper_bound")
		len=v.type.type.size;
		len*=v.type.upper_bound+1;
	else
		len=v.type.size;
	end
	td=do_and_wait(a,sprintf("mdw %d %d\n",v.address,ceil(len/4)));
	b=decode_string(td)(1:len);

	if isfield(v.type, "proc")
		b=v.type.proc(a,v.type,b);
	end

endfunction

function result=decode(info, b)
	switch(info.decode)
	case "structure" result=decode_struct(a,v,b);
	case "enum" result=decode_enum(a,v,b);
	case "union"
		for x=fieldnames(info.type.union)'
			name=x{1}
			result.(name)=decode(info.type.union.(name),b);
		end
	otherwise
		if(strcmp(info.decode,"pointer"))
			result=typecast(b,"uint32");
		else
			result=typecast(b,info.decode);
		end
	end
endfunction

function result=decode_struct(a,v,b)
	for x=fieldnames(v.type.structure)'
		info=v.type.structure.(x{1});
		s=info.data_member_location+1;
		e=s+info.type.size-1;
		result.(x{1})=decode(info,b(s:e));
	end
endfunction

function result=decode_enum(a,v,b)
	result=typecast(b,"uint8");
	for x=fieldnames(v.type.enum)'
		if v.type.enum.(x{1})==result
			result=x{1};
		end
	end
endfunction
