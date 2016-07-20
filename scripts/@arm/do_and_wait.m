function td=do_and_wait(a,command,verbose=0)
	if(command(end)!="\n")
		command(end+1)="\n";
	end
	send(a.local.skt,command);
	td=[];
	do
		usleep(1000);
		[t,len_s]=recv(a.local.skt,10000); 
		if verbose
			printf("%s",t);
			fflush(stdout);
		end
		td=[td, t];
		lines=strsplit(char(td),"\n");
	until(index(lines{end},"\r> "))

	for x=find(cellfun(@length,strfind(lines,"failed")))
		printf("%s\n",lines{x});
	end
endfunction
