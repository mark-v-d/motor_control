function result=set_Kcurrent(a,K,Kl=[])
	if K.tsam!=1/18000
		error("wrong sampling speed");
	elseif(size(K.A,1)==1&& size(K.A,2)==1)
		a.Kcurrent.a=[K.A,0;0,0];
		a.Kcurrent.b=[K.B,0];
		a.Kcurrent.c=[K.C,0];
		a.Kcurrent.d=K.D;
		a.Kcurrent.K=[K.B/K.D, 0];
	elseif size(K.A,1)==2 && size(K.A,2)==2
		a.Kcurrent.a=K.A;
		a.Kcurrent.b=K.B;
		a.Kcurrent.c=K.c;
		a.Kcurrent.d=K.d;
		a.Kcurrent.K=Kl;
	end
endfunction
