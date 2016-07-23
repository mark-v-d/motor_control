function result=program(a)
	[output,text]=system("make -C ../software");
	puts(text);
	if(output)
		error("Compilation failed");
	end

	sprintf("program %s/../software/main.elf\n",pwd)
	do_and_wait(a,sprintf("program %s/../software/main.elf\n",pwd),1);
	do_and_wait(a,"reset\n",1);
	puts("done\n");

	load_symbols(a);

	assignin("caller",inputname(1),a);
endfunction
