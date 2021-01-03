function result=arm(port=4444,filename="../xmc4400/main.elf")
	result.local.skt=socket(AF_INET, SOCK_STREAM, 0);
	client_info = struct("addr", "localhost", "port", port);
	connect(result.local.skt, client_info);
	pause(1e-3)
	#[td,len_s]=recv(result.local.skt,10000);

	result=class(result,"arm");
	result.itm_enabled=0;
	load_symbols(result,filename);
endfunction
