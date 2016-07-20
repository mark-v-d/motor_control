function result=arm()
	result.local.skt=socket(AF_INET, SOCK_STREAM, 0);
	client_info = struct("addr", "localhost", "port", 4444);
	connect(result.local.skt, client_info);
	usleep(1000)
	[td,len_s]=recv(result.local.skt,10000); 

	result=class(result,"arm");
	load_symbols(result);
endfunction
