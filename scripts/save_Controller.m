function [result, sys]=save_Controller(K,limit=1,Kl=[])
	[sys Klimiter]=speedup_limiter(K);
	A=zeros(1,9);
	B=zeros(1,6);
	C=zeros(1,2);
	D=zeros(1,2);
	Kl=zeros(1,3);
	if size(K,1)!=1
		error("Need one output");
	elseif size(K,2)!=2
		error("Need two inputs");
	elseif K.tsam!=1/4500
		error("wrong sampling speed");
	elseif(size(K.A,1)==1&& size(K.A,2)==1)
		A(1)=K.A(1);
		B(1:2)=K.B;
		C(1)=K.C;
		D(1:2)=K.D;
		Kl(1)=Klimiter;
	elseif size(K.A,1)==2 && size(K.A,2)==2
	end

	f=fopen("settings","w");
	fprintf(f,"%f ",A);
	fprintf(f,"\n");
	fprintf(f,"%f ",B);
	fprintf(f,"\n");
	fprintf(f,"%f ",C);
	fprintf(f,"\n");
	fprintf(f,"%f ",D);
	fprintf(f,"\n");
	fprintf(f,"%f ",Kl);
	fprintf(f,"\n");
	fprintf(f,"%f ",limit);
	fclose(f);
endfunction
