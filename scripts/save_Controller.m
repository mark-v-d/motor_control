function [result, sys]=save_Controller(K,limit=3,Klin=[],scale=[1 1],
	#mac="c2:00:85:0c:10:c0"
	#mac="c2:80:12:1c:0d:00"
	mac
)
	#K=K*diag(1./scale)
	if exist("settings","file") && !length(Klin)
		f=fopen("settings","r");
		for i=1:5
			l=fgets(f);
		end
		Klin=sscanf(l,"%e ");
	end
	A=zeros(3,3);
	B=zeros(3,2);
	C=zeros(1,3);
	D=zeros(1,2);
	Kl=zeros(1,3);
	size(K,1)
	if size(K,1)!=1
		error("Need one output");
	elseif size(K,2)!=2 && size(K,1)!=1
		error("Need one or two inputs");
	elseif K.tsam!=1/4500
		error("wrong sampling speed");
	elseif(size(K.A,1)==1&& size(K.A,2)==1)
		[sys Klimiter]=speedup_limiter(K);
		fflush(stdout)
		A(1)=K.A(1);
		B(1:2)=K.B;
		C(1)=K.C;
		D(1:2)=K.D;
		Kl(1)=Klimiter;
	elseif size(K.A,1)==2 && size(K.A,2)==2
		[sys Klimiter]=speedup_limiter(K);
		fflush(stdout)
		A(1:2,1:2)=K.A
		B(1:2,1:2)=K.B
		C(1:2)=K.C
		D=K.D
		Kl(1:2)=Klimiter
	elseif size(K.A,1)==3 && size(K.A,2)==3
		[sys Klimiter]=speedup_limiter(K);
		fflush(stdout)
		A=K.A
		B=K.B
		C=K.C
		D=K.D
		Kl=Klimiter
	end

	f=fopen("settings","w");
	fprintf(f,"%s # mac address\n",mac);
	fprintf(f,"%.10e ",A);
	fprintf(f,"# A\n");
	fprintf(f,"%.10e ",B);
	fprintf(f,"# B\n");
	fprintf(f,"%.10e ",C);
	fprintf(f,"# C\n");
	fprintf(f,"%.10e ",D);
	fprintf(f,"# D\n");
	fprintf(f,"%.10e ",Kl);
	fprintf(f,"# Kl\n");
	fprintf(f,"%.10e ",limit);
	fprintf(f,"# limit\n");
	fprintf(f,"%.10e ",scale);
	fprintf(f,"# scale\n");
	fclose(f);

	f=fopen("settings_linuxcnc.hal","w");
	fprintf(f,"setp statespace_3.0.scale-0		%.10e\n",scale(1));
	fprintf(f,"setp statespace_3.0.scale-1		%.10e\n",scale(2));
	for i=1:3
		for j=1:3
			fprintf(f,"setp statespace_3.0.A-%d-%d		%.10e\n",i-1,j-1,A(i,j));
		end
	end
	for i=1:3
		for j=1:2
			fprintf(f,"setp statespace_3.0.B-%d-%d		%.10e\n",i-1,j-1,B(i,j));
		end
	end
	for j=1:3
		i=1;
		fprintf(f,"setp statespace_3.0.C-%d-%d		%.10e\n",i-1,j-1,C(i,j));
	end
	for j=1:2
		i=1;
		fprintf(f,"setp statespace_3.0.D-%d-%d		%.10e\n",i-1,j-1,D(i,j));
	end
	for i=1:3
		fprintf(f,"setp statespace_3.0.K-%d-0		%.10e\n",i-1,Kl(i));
	end
	fprintf(f,"setp statespace_3.0.limit-0		%.10e\n",limit);
	fclose(f);
endfunction
