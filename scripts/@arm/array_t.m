function result=array_t(a, decode, data)
	result=[];
	for x=0:decode.upper_bound 
		t=decode.type.proc(a,decode.type,data((x*decode.type.size+1):((x+1)*decode.type.size)));
		if(isstruct(t))
			result{end+1}=t;
		else
			result(end+1)=t;
		end
	end
endfunction
