function result=mac_addr(a)
	#s(1).type=".";
	#s(1).subs="g_chipid";
	#result=subsref(a, s);
	result=a.g_chipid;
	result(1)=bitand(254,result(1));
	result(1)=bitor(2,result(1));
	result=sprintf("%02x:",result(1:6));
	result=result(1:end-1);
endfunction
