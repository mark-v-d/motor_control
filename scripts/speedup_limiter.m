function [Klimiting x0 n nev]=speedup_limiter(K,Kinit=[])
	kl={};
	kk=[];
	stables=0;
	p=[];
	attempts=0;
	do
		attempts++
		fflush(stdout);
		if isempty(Kinit)
			Kk=100*(rand(size(K.a,1),size(K,1))-0.5);
		else
			Kk=Kinit;
		end
		Klimiting=K;
		Klimiting.A=K.A-Kk*K.C;
		Klimiting.B=K.B-Kk*K.D;
	until(isstable(Klimiting));
	[x0 n nev]=nelder_mead_min(@get_pole,{Kk,K},"maxev",1e4);

	Kk=x0;
	Klimiting=K;
	Klimiting.A=K.A-Kk*K.C;
	Klimiting.B=K.B-Kk*K.D;
end

function result=get_pole(Kk,K)
	Klimiting=K;
	Klimiting.A=K.A-Kk*K.C;
	Klimiting.B=K.B-Kk*K.D;
	if(isstable(Klimiting)) 
		#kl{end+1}=Klimiting;
		#kk(:,end+1)=Kk;
		result=max(abs(pzmap(Klimiting)));
	else
		result=Inf;
	end;
end
