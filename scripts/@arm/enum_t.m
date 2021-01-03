function result=enum_t(a, decode, data)
	for x=fieldnames(decode.enum)'
		if(data==decode.enum.(x{1}))
			result=x{1};
			return;
		end
	end
	result=data;
endfunction
