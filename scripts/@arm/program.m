function result=program(a)
	[output,text]=system("make -C ../software");
	puts(text);
	if(output)
		error("Compilation failed");
	end

	do_and_wait(a,"program main.elf\n",1);
	do_and_wait(a,"reset\n",1);
	puts("done\n");

	load_symbols(a);

	assignin("caller",inputname(1),a);
endfunction
