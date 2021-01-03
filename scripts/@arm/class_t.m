function result=class_t(a, decode, data)
	result=[];
	for f=fieldnames(decode.structure)'
		fn=f{1};
		if strcmp(fn,"dummy")
			continue;
		end
		x=decode.structure.(fn);
		s=x.data_member_location+1;
		e=s+block_size(a,x)-1;
		result.(fn)=x.type.proc(a,x.type,data(s:e));
	end
endfunction
