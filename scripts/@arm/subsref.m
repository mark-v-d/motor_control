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
			[data, headers{end+1}]=tl_peek(a.local.skt,a.local.ctr,
				start*4,len);
			b=[b,data];
		until x>length(ind);
		b=typecast(b,"uint32");
	case "{}"
		error("sw cannot be indexed with ()");
	case "."
		if strcmp(s(1).subs,"local") || strcmp(s(1).subs,"var")
			b=a.(s(1).subs);
		else
			b=dodot(a,a.var.(s(1).subs));
		end
	end
	if(length(s)>1)
		b=subsref(b,s(2:end));
	end
endfunction

function b=dodot(a,v)
	len=v.type.size;
	if isfield(v.type,"upper_bound")
		len*=v.type.upper_bound+1;
	end
	td=do_and_wait(a,sprintf("mdb %d %d\n",v.address,len)); 
	lines=strsplit(char(td),"\n");
	b=[];
	for x=lines
		line=char(x{1});
		if length(line)<13 || (line(11)!=':' && line(12)!=':')
			continue;
		end
		b=[b;uint8(sscanf(line(13:end),"%x"))];
	end

	switch(v.decode)
	case "structure" b=decode_struct(a,v,b);
	case "enum" b=decode_enum(a,v,b);
	otherwise b=typecast(b,v.decode);
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
