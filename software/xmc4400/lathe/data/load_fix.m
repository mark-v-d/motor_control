function result=load_fix()
	names=ls("*.fix");
	for f=1:size(names,1)
		t=load(strtrim(names(f,:)));
		result{end+1}.name=strtrim(names(f,:));
		result{end}.setpoint=t(1:end-1,1);
		result{end}.iset=t(1:end-1,2);
		result{end}.i=t(1:end-1,3);
		result{end}.pos=t(1:end-1,4);
		result{end}.d=iddata(result{end}.pos,result{end}.i,1/4500);
		result{end}.d.inname{1}="i";
		result{end}.d.outname{1}="pos"; 
		result{end}.d.expname{1}=result{end}.name;
	end
endfunction
